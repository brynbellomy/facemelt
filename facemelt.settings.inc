<?php

/**
 * Fetch the Facebook app id.  Falls back to the FBOAuth module's variables if
 * they happen to be present.
 *
 * @return string
 * @author bryn austin bellomy
 */
function facemelt_facebook_app_id() {
  static $drupal_static_fast;
  if (!isset($drupal_static_fast))
    $drupal_static_fast['app_id'] = &drupal_static(__FUNCTION__);
  $app_id = &$drupal_static_fast['app_id'];
  
  if (!isset($app_id)) {
    $app_id = variable_get('facemelt_fb_app_id', NULL);
    if ($app_id == NULL) {
      $app_id = variable_get('fboauth_id', NULL);
      if ($app_id != NULL) variable_set('facemelt_fb_app_id', $app_id);
    }
  }

  return $app_id;
}

/**
 * Fetch the Facebook app secret.  Falls back to the FBOAuth module's variables
 * if they happen to be present.
 *
 * @return string
 * @author bryn austin bellomy
 */
function facemelt_facebook_app_secret() {
  static $drupal_static_fast;
  if (!isset($drupal_static_fast))
    $drupal_static_fast['secret'] = &drupal_static(__FUNCTION__);
  $secret = &$drupal_static_fast['secret'];
  
  if (!isset($secret)) {
    $secret = variable_get('facemelt_fb_app_secret', NULL);
    if ($secret == NULL) {
      $secret = variable_get('fboauth_secret', NULL);
      if ($secret != NULL)
        variable_set('facemelt_fb_app_secret', $sec);
    }
  }

  return $secret;
}

/**
 * Fetch the URL to the channel.php/channel.html file that allows Facebook to
 * perform cross-domain requests without violating Javascript access policies.
 *
 * @return string
 * @author bryn austin bellomy
 */

function facemelt_fb_channel_url() {
  static $url;
  if (!isset($url))
    $url = variable_get('facemelt_fb_channel_url', 'facemelt/channel');
  return url($url, array('absolute' => TRUE));
}

/*function facemelt__redirect_uri($absolute = FALSE) {
  return url(variable_get('atdaa_facebook_login_redirect_uri', 'facemelt/connect'), array('absolute' => $absolute));
}*/



/**
 * Automatically initiate Drupal login when an authenticated Facebook session
 * exists, no matter what page the user happens to be on.  When FALSE, the user
 * must navigate to the URL specified in 'facemelt_fbconnect_fb_manual_connect_path'
 * for the Drupal login flow to begin.
 * @values TRUE/FALSE
 */
/*function facemelt_fbconnect_auto_connect() {
  return variable_get(__FUNCTION__, FALSE);
}*/

/**
 * This is the page on which we wait for the auth.login event from Facebook
 */
function facemelt_fbconnect_receive_authenticated_session_path($absolute = TRUE) {
  $url = variable_get(__FUNCTION__, 'facemelt/connect/login');
  if (!$absolute) return $url;
  else return url($url, array('absolute' => $absolute));
}

/**
 * Path to redirect to after the Connect user flow has completed.  This should
 * generally try to close the Facebook popup if there is one.
 */
function facemelt_fbconnect_redirect_path($absolute = TRUE) {
  static $url;
  if (!isset($url))
    $url = variable_get(__FUNCTION__, 'facemelt/connect/close-popup'); //'fboauth/connect');//
  
  return url($url,
    array(
      'absolute' => $absolute,
      /*'query' => array(
        'destination' => facemelt_fbconnect_receive_authenticated_session_path(FALSE)
      ),*/
    )
  );
}

function facemelt_fbconnect_some_pages_require_connect() {
  return variable_get(__FUNCTION__, FALSE);
}

function facemelt_fbconnect_paths_requiring_connect() {
  return variable_get(__FUNCTION__, array());
}

function facemelt_fbconnect_connect_required_message() {
  return variable_get(__FUNCTION__, NULL);
}

function facemelt_fbconnect_connect_required_redirect_path() {
  return variable_get(__FUNCTION__, NULL);
}

