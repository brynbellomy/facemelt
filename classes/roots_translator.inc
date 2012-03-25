<?php

abstract class roots_translator {
  
  abstract public function __construct($foreign_data, $parent_object = NULL);
  
  public static function translate($src_data, $src_handler_class, $dst_handler) {
    $data_copy = (array)$src_data;
    $return = new stdClass;
    $is_array = is_array($data);
    $src_data = (object)unserialize(serialize($src_data)); // clone and break all lingering references
    
    $src_type = call_user_func(array($src_handler_class, 'get_type'));
    
    //foreach ($this->get_translations($src_type) as $src_key => $dest_key) {
    foreach ($dst_handler->get_translations($src_type) as $src_key => $dest_key) {
      if ($src_data->$src_key) {
        $return->$dest_key = $dst_handler->translate_field($dest_key, $src_data->$src_key);
        /*$function = "translate_field_$dest_key";
        if (is_callable(array($dst_handler, $function))) {
          $return->$dest_key = call_user_func(array($dst_handler, $function), $src_data->$src_key);
        }
        else {
          $return->$dest_key = $src_data->$src_key;
        }*/
        unset($src_data->$src_key); // @@TODO: if we don't use the untranslateable thing, then we don't need this, right?
      }
    }
    
    // convert data back to original type
    if ($is_array) {
      //$data = (array)$data;
      $return = (array)$return;
    }
    
    //$this->setUntranslateable($data); // give the extending class the opportunity to handle untranslateable items
    return $return;
  }
  
  /*protected function setUntranslateable($data = NULL) {
    static $untranslateable = NULL;
    if ($data) {
      $untranslateable = $data;
    }
    return $untranslateable;
  }
  
  protected function getUntranslateable() {
    return $this->setUntranslateable();
  }*/
}