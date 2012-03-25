<?php

module_load_include('inc', 'smart_helper', 'classes/sl_ya_object');

class sl_charity extends sl_ya_object {
  /* yogaaid feed fields:
      - guid
      - name
      - thumbnail
      - bio
      - website url
  */
  
  public function insert() {
    parent::insert();
    
    if ($this->ya_guid) {
      variable_set('sl_eventfeed_CharityID_OffTheMat', $this->ya_guid);
    }
  }
  
  public function update() {
    parent::update();
    
    if ($this->ya_guid) {
      variable_set('sl_eventfeed_CharityID_OffTheMat', $this->ya_guid);
    }
  }
  
  protected function table_name() {
    return SL_CHARITIES_TABLE;
  }

  protected function primary_key_field() {
    return 'ya_guid';
  }
  
  /** roots_translateable implementation **/
  function get_translations($src_type = NULL) {
    if ($src_type == ya_feed::get_type()) {
      return array(
        'CharityID' => 'ya_guid',
        'Name' => 'name',
        'Thumb' => 'thumbnail',
        'Bio' => 'bio',
        'Website' => 'website',
      );
    }
  }
  
  /** roots_mockable implementation **/
  public function generate_mock_data($field) {
    /*switch ($field) {
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
    }*/
    
    return parent::generate_mock_data($field); // default if unhandled field
  }
}