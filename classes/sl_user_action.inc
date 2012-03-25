<?php

module_load_include('inc', 'smart_helper', 'classes/sl_object');

class sl_user_action extends sl_object {
  protected function table_name() {
    return SL_USERS_ACTIONS_TABLE;
  }

  protected function primary_key_field() {
    return FALSE;
  }
  
  protected function keys() {
    return array('uid', 'aid');
  }
  
  /** roots_mockable implementation **/
  public function generate_mock_data($field) {
    switch ($field) {
      case 'uid':
        // bryn's friends
        if ($_GET['friends-of'] == 'bryn') {
          return array('5741', '13891', '20300', '20323', '20373', '22769', '108949', '309843', '601986', '617801', '619665', '822981', '827442', '836887', '908908', '919626');
        }
        // shim's friends
        return array(13132,23844,100341,100472,120429,300038,300683,307432,410889,416728,419669,506528,603915,604140,605932,608043,611711,706827,802235,802672,804413,812651,813803,815785,833053,844709,902494,905201,906857,1001077,1100057,1235966,1400046,1400076,1400246,1400312,1400353,1400522,1400523,1400530);
      case 'aid':
        return array_keys(smart_helper_get_actions());
      case 'timestamp':
        return range(time() - (14 * 24 * 60 * 60), time() - 60);
    }
    
    return parent::generate_mock_data($field); // default if unhandled field
  }
}