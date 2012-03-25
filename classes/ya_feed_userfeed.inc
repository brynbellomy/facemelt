<?php

module_load_include('inc', 'smart_helper', 'classes/ya_feed');

class ya_feed_userfeed extends ya_feed {
  protected function endpoint() {
    //return '/wlf - userfeed json.txt';  // @@TODO: remove after launch -- for bryn's local environment debug purposes only
    //return 'http://www.yogaaid.com/USA/userfeed.json';
    return 'http://www.yogaaid.com/wanderlust/userfeed.json';
    //return '/home/rootdown/temp/wl-userfeed.txt';  // @@TODO: remove after launch -- for webfaction environment debug only
  }

  protected function objects_to_store() {
    return array(
      'UserList' => array(
        'handler' => 'sl_user',
        'children' => array(
          'DonationActivityList' => array(
            'handler' => 'sl_donation',
          ),
        ),
      ),
    );
  }

  protected function globals_to_store() {
    return array(
      'userfeed' => array(),
    );
  }
  
  
  
  // @@TODO
  function get_translations($src_type = NULL) {
  }
}
