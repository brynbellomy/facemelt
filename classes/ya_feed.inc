<?php

module_load_include('inc', 'smart_helper', 'classes/roots_translateable');
module_load_include('inc', 'smart_helper', 'classes/roots_remote_data_handler');

abstract class ya_feed extends roots_remote_data_handler implements roots_translateable {
  static function get_type() {
    return 'yogaaid feed';
  }
  
  function get_data($field = NULL) {
    if (isset($this->foreign_data[$field])) {
      return $this->foreign_data[$field];
    }
    return NULL;
  }
  
  public function translate_field($field_name, $data) {
    return $data;
  }
}