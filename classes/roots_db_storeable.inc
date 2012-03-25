<?php

module_load_include('inc', 'smart_helper', 'classes/roots_mockable');

abstract class roots_db_storeable implements roots_mockable {
  private $__is_mock_object;

  public function __construct($base = NULL, $parent_object = NULL) {
    if ($base == NULL) {
      return;
    }
    
    $this->__is_mock_object = FALSE;

    if (is_numeric($base) || is_string($base)) {
      if ($this->primary_key_type() == 'int') {
        //$check_function = 'is_int';
        $test = (int)$base;
        if (empty($test)) {
          return;
        }
        $base = (int)$base;
      }
      else if ($this->primary_key_type() == 'string') {
        //$check_function = 'is_string';
        if (!is_string($base) || strlen($base) <= 0) {
          return;
        }
        $base = (string)$base;
      }
    }
    
    //if ($check_function($base)) { // if $base appears to be a primary key value, look the object up from the db
    if (!$this->load($base)) {
      if ($primary_key = $this->primary_key_field()) { // this only works if this table has a primary key (relationship tables can't use this feature)
        $this->$primary_key = NULL; // ensure it's clear to child constructors that the object is not initialized
      }
    }
    //}
    /*else {
      // if $base isn't a primary key value, try to use roots_translator to interpret it
      $data = (array)$this->translate($base);
      foreach ($this->my_fields() as $field) {
        if ($data[$field]) {
          $this->$field = $data[$field];
        }
        else if (in_array($field, array_keys((array)$this->getUntranslateable()))) {
          // @@TODO: some kind of error?
        }
      }
    }*/
  }

  /** dynamic scaffolding functions **/
  protected function my_fields() {
    $table = data_get_table($this->table_name());
    $schema = $table->get('table_schema');
    return array_keys($schema['fields']);
  }

  protected function field_data_type($field) {
    $table_name = $this->table_name();
    $table = data_get_table($this->table_name());
    $schema = $table->get('table_schema');
    if ($schema['fields'][$field]) {
      if (in_array($schema['fields'][$field]['type'], array('int', 'float', 'serial'))) { // @@TODO: handle floats separately
        return 'int';
      }
      else if (in_array($schema['fields'][$field]['type'], array('varchar', 'text'))) {
        return 'string';
      }
      return FALSE;
    }
  }

  public function __get($field) {
    if (in_array($field, $this->my_fields())) {
      if (!isset($this->$field)) {
        $this->$field = NULL;
      }
      return $this->$field;
    }
    return NULL;
  }

  public function __set($field, $value) {
    if (in_array($field, $this->my_fields())) { // only permit fields that accord with the db schema to be set
      // do automatic type discovery and casting
      if ($this->field_data_type($field) == 'int') {
        $this->$field = (int)$value;
      }
      else if ($this->field_data_type($field) == 'string') {
        $this->$field = (string)$value;
      }
    }
  }

  /** db functions **/
  public function load($base, $return = FALSE) {
    $table = data_get_table($this->table_name());
    $class = get_called_class();
    $ghost = new $class;

    if (is_array($base)) { // lookup based on any number of key => value pairs
      $lookup_params = $base;
    }
    else if (is_string($base) || is_numeric($base)) { // lookup based on primary key
      if ($primary_key = $this->primary_key_field()) {
        if ($this->primary_key_type() == 'int') {
          $lookup_params = array($primary_key => (int)$base);
        }
        else if ($this->primary_key_type() == 'string') {
          $lookup_params = array($primary_key => (string)$base);
        }
        else {
          $lookup_params = array($primary_key => $base);
        }
      }
      else {
        throw new Exception('Cannot perform primary key lookup on a model that does not specify a primary key.', SL_EXCEPTION_MODEL_HAS_NO_PRIMARY_KEY);
      }
    }

    $result = $table->handler()->load($lookup_params);
    if (is_array($result) && count($result) > 0) {
      $obj = $result[0];
      foreach ($obj as $field => $val) {
        $ghost->$field = $val;
      }

      $mock_objs = variable_get('sl_mock_objects', array());
      if (isset($mock_objs[$this->table_name()]) && in_array($ghost->$primary_key, $mock_objs[$this->table_name()])) {
        $ghost->__is_mock_object = TRUE;
      }
      else {
        $ghost->__is_mock_object = FALSE;
      }

      if ($return) {
        return $ghost;
      }
      else {
        foreach ($ghost as $field => $val) {
          $this->$field = $val;
        }
        return $this;
      }
    }
    else {
      if ($return) {
        return FALSE;
      }
      else {
        throw new Exception('Could not find object in database.', SL_EXCEPTION_OBJECT_NOT_FOUND_IN_DB);
      }
    }
  }
  
  protected function enough_keys_filled_in_for_save() {
    $at_least_one_key_set = FALSE;
    $update = array();
    foreach ($this->keys() as $key) {
      if (isset($this->$key)) {
        $update[] = $key;
        $at_least_one_key_set = TRUE;
      }
    }
    return $at_least_one_key_set;
  }

  public function insert() {
    /* @@TODO:
       how to guard against inserting rows with fb_uid = 0?
       1) if primary key is not serial, it must be filled in
    if (!$this->enough_keys_filled_in_for_save()) {
      throw new Exception('Cannot insert object because none of its keys contain a value.', SL_EXCEPTION_OBJECT_HAS_NO_KEYS);
    }*/
    $table = data_get_table($this->table_name());
    $success = $table->handler()->save($this);
    if (!$success) {
      throw new Exception('Database operation failed.', SL_EXCEPTION_DB_OPERATION_FAILED);
    }

    if ($this->__is_mock_object === TRUE) {
      $mock_objs = variable_get('sl_mock_objects', array());
      if (!isset($mock_objs[$this->table_name()])) {
        $mock_objs[$this->table_name()] = array();
      }
      $primary_key = $this->primary_key_field();
      $mock_objs[$this->table_name()][] = $this->$primary_key;
      variable_set('sl_mock_objects', $mock_objs);
    }
    return $this;
  }

  public function exists_in_db() {
    $lookup = array();
    
    if ($keys = $this->keys()) {
      foreach ($keys as $key) {
        if (isset($this->$key)) {
          $lookup[$key] = $this->$key;
        }
      }
    }
    else {
      throw new Exception('Model has no keys.', SL_EXCEPTION_MODEL_HAS_NO_KEYS);
    }
    
    if (count($lookup) <= 0) {
      throw new Exception('Cannot lookup object because none of its keys contain a value.', SL_EXCEPTION_OBJECT_HAS_NO_KEYS);
    }
    
    if ($this->load($lookup, TRUE)) { // check if object exists
      return TRUE;
    }
    return FALSE;
  }

  public function update() {
    $table = data_get_table($this->table_name());

    // gripe if the model specifies no keys
    if (!$this->keys()) {
      throw new Exception('Model has no keys.', SL_EXCEPTION_MODEL_HAS_NO_KEYS);
    }
    
    // make sure that at least one of the keys has a value so we have SOME chance of saving the object
    if (!$this->enough_keys_filled_in_for_save()) {
      throw new Exception('Cannot update object because none of its keys contain a value.', SL_EXCEPTION_OBJECT_HAS_NO_KEYS);
    }

    $success = $table->handler()->update($this, $this->keys());
    if (!$success) {
      throw new Exception('Database operation failed.', SL_EXCEPTION_DB_OPERATION_FAILED);
    }

    if ($this->__is_mock_object === TRUE) {
      $mock_objs = variable_get('sl_mock_objects', array());
      if (!isset($mock_objs[$this->table_name()])) {
        $mock_objs[$this->table_name()] = array();
      }
      $primary_key = $this->primary_key_field();
      $mock_objs[$this->table_name()][] = $this->$primary_key;
      variable_set('sl_mock_objects', $mock_objs);
    }
    return $this;
  }
  
  public function delete($where = NULL) {
    if (!$where) {
      // gripe if the model specifies no keys
      if (!$this->keys()) {
        throw new Exception('Model has no keys.', SL_EXCEPTION_MODEL_HAS_NO_KEYS);
      }
    
      $where = array();
      foreach ((array)$this->keys() as $key) {
        if ($this->$key) {
          $where[$key] = $this->$key;
        }
      }
      
      // make sure that at least one of the keys has a value so we have SOME chance of finding and deleting the object
      if (count($where) <= 0) { 
        throw new Exception('Cannot delete object because none of its keys contain a value and no WHERE clause was specified.', SL_EXCEPTION_OBJECT_HAS_NO_KEYS);
      }
    }
    
    $table = data_get_table($this->table_name());
    $affected_rows = $table->handler()->delete($where);
    if (!$affected_rows) {
      throw new Exception('No rows affected by DELETE query.', SL_EXCEPTION_NO_AFFECTED_ROWS);
    }
    return $this;
  }

  protected function primary_key_type() {
    return 'int'; // safe to assume for most kinds of objects
  }

  abstract protected function table_name();
  abstract protected function primary_key_field();

  protected function keys() {
    return array($this->primary_key_field());
  }

  /** implementation of roots_mockable **/
  public function generate_mock_object() {
    foreach ($this->my_fields() as $field) {
      $possibles = $this->generate_mock_data($field);
      $this->$field = $possibles[rand(0, count($possibles) - 1)];
    }
    $this->__is_mock_object = TRUE;
    return $this;
  }

  public function generate_mock_data($field) {
    if ($this->field_data_type($field) == 'string') {
      return '';
    }
    else if ($this->field_data_type($field) == 'int') {
      return 0;
    }
  }

  public static function generate_mock_objects($num) {
    $objects = array();
    $class = get_called_class();
    foreach (range(0, $num) as $i) {
      $obj = new $class;
      $obj->generate_mock_object()->insert();
      $objects[] = $obj;
    }
    return $objects;
  }
}