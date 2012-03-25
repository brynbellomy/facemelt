<?php

module_load_include('inc', 'smart_helper', 'classes/sl_ya_object');

class sl_donation extends sl_ya_object {
  /* yogaaid feed fields:
      - name
      - date
      - amount
      - comment
  */
  
  public function __construct($base = NULL, $parent_object = NULL) {
    parent::__construct($base, $parent_object);
    
    // record the fb_uid of the user to whom this donation was made
    $this->to_uid = $parent_object->fb_uid;
  }
  
  public function __set($field, $value) {
    if ($field == 'amount') {
      $value = (float)preg_replace('/[^0-9\.]/', '', (string)$value);
      $this->$field = (float)money_format('%!n', $value);
      return;
    }

    parent::__set($field, $value);
  }
  
  protected function table_name() {
    return SL_DONATIONS_TABLE;
  }

  protected function primary_key_field() {
    return 'did';
  }
  
  public function exists_in_db() {
    // Donation objects come from YogaAid but do not have a GUID, so we have to try to look them up via their other parameters
    if (!$this->did) {
      $lookup = array();
      if (isset($this->to_uid)) {
        $lookup['to_uid'] = (string)$this->to_uid;
      }
      if (isset($this->from_name)) {
        $lookup['from_name'] = $this->from_name;
      }
      if (isset($this->timestamp)) {
        $lookup['timestamp'] = (int)$this->timestamp;
      }
      if (isset($this->amount)) {
        $lookup['amount'] = (string)$this->amount;
      }
      if ($this->load($lookup, TRUE)) {
        return TRUE;
      }
    }
    else {
      return parent::exists_in_db();
    }
        
    return FALSE;
  }
  
  /** roots_translateable implementation **/
  public function get_translations($src_type = NULL) {
    if ($src_type == ya_feed::get_type()) {
      return array(
        'Name' => 'from_name',
        'Amount' => 'amount',
        'Date' => 'timestamp',
        'Comment' => 'comment',
      );
    }
  }
  
  public function translate_field($field_name, $data) {
    if ($field_name == 'timestamp') {
      return strtotime((string)$data);
    }
    else {
      return parent::translate_field($field_name, $data);
    }
  }
  
  /** roots_mockable implementation **/
  public function generate_mock_data($field) {
    switch ($field) {
      case 'amount':
        return range(10,200);
      case 'from_uid':
        return array('5741', '13891', '20300', '20323', '20373', '22769', '108949', '309843', '601986', '617801', '619665', '822981', '827442', '836887', '908908', '919626');
      case 'to_uid':
        $table = SL_USERS_TABLE;
        $q = "SELECT fb_uid FROM {{$table}}";
        $res = db_query($q);
        while ($row = db_result($res)) {
          $fb_uids[] = (float)$row;
        }
        return $fb_uids;
    }
    
    return parent::generate_mock_data($field); // default if unhandled field
  }
}
