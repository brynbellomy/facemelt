<?php

module_load_include('inc', 'smart_helper', 'classes/sl_ya_object');

class sl_location extends sl_ya_object {
  /*var $guid;
  var $amount_raised;
  var $num_challengers;
  var $thumbnail_img;
  var $name;*/

  protected function table_name() {
    return SL_LOCATIONS_TABLE;
  }

  protected function primary_key_field() {
    return 'guid';
  }
  
  protected function primary_key_type() {
    return 'string';
  }
  
  /** roots_translateable implementation **/
  function get_translations($src_type = NULL) {
    if ($src_type == ya_feed::get_type()) {
      return array(
        'LocationID' => 'guid',
        'AmountRaised' => 'amount_raised',
        'ChallengersNum' => 'num_challengers',
        'Thumb' => 'thumbnail_img',
        'Location' => 'name',
      );
    }
  }
}