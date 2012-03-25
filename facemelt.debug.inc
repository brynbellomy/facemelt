<?php


/**
 * Handle debug options that can be processed before Facebook is initialized.
 *
 * @return void
 * @author bryn bellomy
 */
function facemelt_handle_absolute_first_debug_options() {
  if (user_access('use facemelt debug mode')) {
    if ($_GET['debug-mode'] == 1)
      variable_set('facemelt_debug_mode', TRUE);
    else if ($_GET['debug-mode'] === '0')
      variable_set('facemelt_debug_mode', FALSE);
  
    if (facemelt_debug_mode())
      dsm(t('Debug mode is currently active.  To disable, add "?debug-mode=0" to any URL.'));
  }
}

/**
 * Handle debug options which must be processed after the Facebook
 * API object is initialized but before the user is logged in.
 *
 * @return void
 * @author bryn austin bellomy
 */
function facemelt_handle_pre_login_debug_options() {
  if (facemelt_debug_mode()) {
    if ($_GET['create-test-user'] == 1) {
      if ($test_user = facemelt_fb_create_test_user(FALSE)) {
        facemelt_dsm($test_user, t('New Facebook test user created'));
      }
    }

    if ($_GET['show-test-users'] == 1) {
      if ($test_users = facemelt_fb_show_test_users()) {
        facemelt_dsm($test_users, t('Facebook test users'));
      }
    }
  }
}

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
      facemelt_dsm(facemelt_me(), t('me'));
    }
    if ($_GET['try-delete-post'] == 1 && $_GET['delete-obj-graph-id']) {
      $facebook = facemelt_fb();
      $response = facemelt_fb_graph_request($_GET['delete-obj-graph-id'], 'DELETE', array('method'=>'delete'), FALSE);
      facemelt_dsm($response, t('delete api response'));
    }
  }
}


