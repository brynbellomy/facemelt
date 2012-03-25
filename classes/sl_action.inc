<?php

module_load_include('inc', 'smart_helper', 'classes/sl_object');

class sl_action extends sl_object {
  /* action fields:
      - aid
      - nid
      - weight
      - parent_aid
  */
  
  /* @@TODO: 
  function __construct($attach_children = FALSE) {
    parent::__construct();
    
    if ($attach_children) {
      smart_helper_get_children_of_action()
    }
  }
  */
  
  function __set($field_name, $value) {
    parent::__set($field_name, $value);

    // allow the fields on the node associated with this sl_action to be grafted onto this object
    if ((strpos($field_name, 'field_') === 0) || in_array($field_name, array('nid', 'title'))) {
      $this->$field_name = $value;
    }
  }
  
  protected function table_name() {
    return SL_ACTIONS_TABLE;
  }

  protected function primary_key_field() {
    return 'aid';
  }
  
  protected function keys() {
    return array('aid', 'nid');
  }
}