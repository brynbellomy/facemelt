<?php

module_load_include('inc', 'smart_helper', 'classes/sl_object');
module_load_include('inc', 'smart_helper', 'classes/roots_translateable');

abstract class sl_ya_object extends sl_object implements roots_translateable {
  
  function __construct($base = NULL, $parent_object = NULL) {
    try {
      parent::__construct($base, $parent_object);
    } catch (Exception $e) {
    }
    
    $primary_key = $this->primary_key_field();
    if (!($this->$primary_key) && $base) {
      // if parent::__construct didn't do anything (i.e., $base isn't a primary key value), try to use roots_translator to interpret it
      $data = (array)roots_translator::translate($base, 'ya_feed', $this);
      foreach ($this->my_fields() as $field) {
        if ($data[$field]) {
          $this->$field = $data[$field];
        }
      }
    }
  }

  static function get_type() {
    return 'sl object';
  }
  
  function get_data($field = NULL) {
    return $this->$field;
  }
  
  public function translate_field($field_name, $data) {
    return $data;
  }
}