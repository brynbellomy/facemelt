<?php

module_load_include('inc', 'smart_helper', 'classes/ya_feed');

class ya_feed_eventfeed extends ya_feed {
  protected function endpoint() {
    //return '/wlf - eventfeed json.txt'; // @@TODO: remove after launch -- for bryn's local environment debug purposes only
    return 'http://www.yogaaid.com/wanderlust/eventfeed.json';
    //return 'http://dev.yogaaid.com/wanderlust/eventfeed.json';
    //return '/home/rootdown/temp/wl-eventfeed.txt';  // @@TODO: remove after launch -- for webfaction environment debug only
  }

  protected function objects_to_store() {
    return array(
      'LocationsList' => array(
        'handler' => 'sl_location',
      ),
      'CharityList' => array(
        'handler' => 'sl_charity',
      ),
    );
  }

  protected function globals_to_store() {
    return array(
      'sl_eventfeed' => array('AmountRaised', 'EventName', 'EventDesc', 'EventID', 'DateString', 'ChallengerNum',),
      );
  }
  
  
  
  // @@TODO
  function get_translations($src_type = NULL) {
  }
}