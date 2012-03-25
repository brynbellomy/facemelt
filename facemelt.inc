<?php

/**
 * Include the official Facebook SDK so we can monkey-patch it into oblivion.
 *
 * @return object
 * @author bryn austin bellomy
 */
function facemelt_require_php_sdk() {
  require_once drupal_get_path('module', 'facemelt') . '/facebook-php-sdk/src/facebook.php';
}

/**
 * Return the global Facebook API object.  Initialize it first if needed.
 *
 * @return object
 * @author bryn austin bellomy
 */
function &facebook($require_session = TRUE) {
  $facebook = &ctools_static('facemelt : facebook : sdk object', NULL);
  if (!$facebook) {
    facemelt_start_facebook($require_session);
  }
  return $facebook;
}

/**
 * Return a Facebook object representing the currently logged-in user.
 *
 * @return object
 * @author bryn austin bellomy
 */
function &facebook_me($require_session = TRUE) {
  $me = &ctools_static('facemelt : facebook : me', NULL);
  if ($me == NULL) {
    facebook($require_session);
  }
  return $me;
}

/**
 * Fetch the Facebook app id.  Falls back to the FBOAuth module's variables if
 * they happen to be present.
 *
 * @return string
 * @author bryn austin bellomy
 */
function facemelt_facebook_app_id() {
  $id = variable_get('facemelt_fb_app_id', NULL);
  if ($id == NULL) {
    $id = variable_get('fboauth_id', NULL);
    if ($id != NULL) variable_set('facemelt_fb_app_id', $id);
  }
  return $id;
}

/**
 * Fetch the Facebook app secret.  Falls back to the FBOAuth module's variables
 * if they happen to be present.
 *
 * @return string
 * @author bryn austin bellomy
 */
function facemelt_facebook_app_secret() {
  $sec = variable_get('facemelt_fb_app_secret', NULL);
  if ($sec == NULL) {
    $sec = variable_get('fboauth_secret', NULL);
    if ($sec != NULL) variable_set('facemelt_fb_app_secret', $sec);
  }
  return $sec;
}


/**
 * Initialize the Facebook API object and set up the SESSION cache.
 *
 * @return TRUE if successfully initialized, FALSE otherwise.
 * @author bryn austin bellomy
 */
function facemelt_start_facebook($require_session = TRUE) {
  facemelt_require_php_sdk();
  module_load_include('inc', 'facemelt', 'classes/facemelt_facebook');
  
  $facebook = &ctools_static('facemelt : facebook : sdk object', NULL);
  $me = &ctools_static('facemelt : facebook : me', NULL);

  // create the application instance
  $facebook = new facemelt_facebook(array(
    'appId'  => facemelt_facebook_app_id(),
    'secret' => facemelt_facebook_app_secret(),
    'cookie' => TRUE,
  ));
  
  $me = NULL;
  $session = $facebook->getSession();
  if ($session || !$require_session) {
    try {
      if ($_me = $facebook->api('/me')) {
        $_me = (object)$_me;
        $_me->name = strtolower($_me->name);
        $_me->first_name = strtolower($_me->first_name);
        $_me->middle_name = strtolower($_me->middle_name);
        $_me->last_name = strtolower($_me->last_name);
        $me = $_me;
        if (function_exists('context_set'))
          context_set('facemelt', 'my fb uid', $me->id);
      }
      
      // if the user isn't logged in to facebook, clear the entries in the SESSION cache that are facebook-dependent (my friends, etc)
      if (!$me) {
        $_SESSION['fbook-dependent'] = array();
        unset($_SESSION['registered-email-address']);
      }
      return TRUE;
    } catch (FacebookApiException $e) {
      // if the user isn't logged in to facebook, clear the entries in the SESSION cache that are facebook-dependent (my friends, etc)
      if (!$me) {
        $_SESSION['fbook-dependent'] = array();
        unset($_SESSION['registered-email-address']);
      }
      
      // if this is an AJAX request, allow the AJAX handler function to redirect the user through login
      if (facemelt_is_ajax_request()) { 
        throw $e;
      }

      if (facemelt_debug_mode()) {
        facemelt_dsm($e, 'Facebook Exception');
      }
      facemelt_watchdog_exception($e, __FUNCTION__, 'Could not initialize Facebook object.');
      return FALSE;
    }
  }
  else {
    return FALSE;
  }
}

