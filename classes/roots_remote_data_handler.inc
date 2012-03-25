<?php

abstract class roots_remote_data_handler {
  private $foreign_data;
  
  /** chainable actions **/
  public function retrieve() {
    $this->foreign_data = file_get_contents($this->endpoint());
    return $this;
  }
  
  public function decode() {
    $this->foreign_data = json_decode($this->foreign_data, TRUE, 512);
    return $this;
  }

  public function store($update_when_possible = FALSE) {
    if ($this->foreign_data) {
      $this->store_objects($this->foreign_data, $update_when_possible);
    
      foreach ($this->globals_to_store() as $namespace => $vars) {
        foreach ($vars as $var) {
          if ($this->foreign_data[$var]) {
            $this->store_global($namespace, $var, $this->foreign_data[$var]);
          }
        }
      }
    }
    
    return $this;
  }
  
  protected function store_objects($foreign_data, $update_when_possible = FALSE, $parent_foreign_key_path = array(), $parent_object = NULL) {
    foreach ($foreign_data as $foreign_key => $objects) {
      $foreign_key_path = $parent_foreign_key_path;
      $foreign_key_path[] = $foreign_key;
      if ($this->can_handle_foreign_key_path($foreign_key_path)) {
        foreach ($objects as $object) {
          $stored_object = $this->store_object($object, $foreign_key_path, $parent_object, $update_when_possible);
          if ($children = $this->children_of_object($object, $foreign_key_path)) {
            $this->store_objects($children, $update_when_possible, $foreign_key_path, $stored_object);
          }
        }
      }
    }
  }
  
  protected function store_object($foreign_object, $foreign_key_path, $parent_object = NULL, $update_when_possible = FALSE) {
    if ($handler_class = $this->handler_for_foreign_key_path($foreign_key_path) and class_exists($handler_class)) {
      module_load_include('inc', 'smart_helper', "classes/$handler_class");
      $handler = new $handler_class($foreign_object, $parent_object);

      try {
        if ($update_when_possible) {
          if ($handler->exists_in_db()) {
            $handler->update(); // @@TODO: check return value, throw exception?  quiet log?
          }
          else {
            $handler->insert(); // @@TODO: check return value, throw exception?  quiet log?
          }
        }
        else {
          $handler->insert(); // @@TODO: check return value, throw exception?  quiet log?
        }
      } catch (Exception $e) {
        //dsm($e, 'Exception caught.'); // @@TODO: log some other way or at least use function_exists()
        if ($e->getCode() == SL_EXCEPTION_DB_OPERATION_FAILED) {

          // @@TODO: maybe log this somewhere and move on with life?
        }
      }
      return $handler;
    }
    else {
      throw new Exception("Handler class \"$handler_class\" not found", SL_EXCEPTION_HANDLER_CLASS_NOT_FOUND);
    }
  }
  
  protected function can_handle_foreign_key_path($foreign_key_path, $return_leaf = FALSE) {
    $handleable_paths = $this->objects_to_store();
    $ptr = &$handleable_paths;
    while ($key = array_shift($foreign_key_path)) {
      if (isset($ptr[$key])) {
        if (count($foreign_key_path) > 0) {
          if (isset($ptr[$key]['children'])) {
            $ptr = &$ptr[$key]['children'];
          }
          else {
            return FALSE;
          }
        }
        else {
          $ptr = &$ptr[$key];
        }
      }
      else {
        return FALSE;
      }
    }
    
    if ($return_leaf) {
      return $ptr;
    }
    else {
      return TRUE;
    }
  }
  
  
  protected function handler_for_foreign_key_path($foreign_key_path) {
    $leaf = $this->can_handle_foreign_key_path($foreign_key_path, TRUE);
    if ($leaf and isset($leaf['handler'])) {
      return $leaf['handler'];
    }
    return FALSE;
  }
  
  
  protected function children_of_object($foreign_data, $object_foreign_key_path) {
    $children = array();
    foreach ($foreign_data as $foreign_key => $val) {
      $foreign_key_path = $object_foreign_key_path;
      $foreign_key_path[] = $foreign_key;
      if ($this->can_handle_foreign_key_path($foreign_key_path) && is_array($val)) {
        $children[$foreign_key] = $val;
      }
    }
    return $children;
  }
  
  protected function store_global($namespace, $key, $value) {
    variable_set("{$namespace}_{$key}", $value);
    return $this;
  }
  
  /** definitions **/
  abstract protected function endpoint();
  abstract protected function objects_to_store();
  abstract protected function globals_to_store();
}
