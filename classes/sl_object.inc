<?php

module_load_include('inc', 'smart_helper', 'classes/roots_db_storeable');

abstract class sl_object extends roots_db_storeable {
  public static function generate_mock_objects($num) {
    $objects = parent::generate_mock_objects($num);
    if (sl_debug_mode()) {
      $class = get_called_class();
      dsm($objects, "Generated mock $class objects");
    }
  }
}