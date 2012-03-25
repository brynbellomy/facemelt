<?php

/**
 * Handle debug options that must be processed after the user is
 * logged into Facebook.
 *
 * @return void
 * @author bryn austin bellomy
 */
function facemelt_handle_post_login_debug_options() {
  if (facemelt_debug_mode()) {
    if ($_GET['show-me'] == 1) {
      facemelt_dsm(facebook_me(), 'me');
    }
    if ($_GET['try-delete-post'] == 1 && $_GET['delete-obj-graph-id']) {
      $facebook = facebook();
      $response = smart_helper_fb_graph_request($_GET['delete-obj-graph-id'], 'DELETE', array('method'=>'delete'), FALSE);
      facemelt_dsm($response, 'delete');
    }
  }
}


