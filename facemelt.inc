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
function &facemelt_fb($require_session = TRUE) {
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
function &facemelt_me($require_session = TRUE) {
  $me = &ctools_static('facemelt : facebook : me', NULL);
  if ($me == NULL) {
    facemelt_fb($require_session);
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
  global $cachemoney_prefix;

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
        $_SESSION[$cachemoney_prefix] = array();
        unset($_SESSION['registered-email-address']);
      }
      return TRUE;
    } catch (FacebookApiException $e) {
      // if the user isn't logged in to facebook, clear the entries in the SESSION cache that are facebook-dependent (my friends, etc)
      if (!$me) {
        $_SESSION[$cachemoney_prefix] = array();
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

/**
 * Post to the current user's wall.
 *
 * @param string $main_text
 *   Main string of text / headline.
 * @param string $secondary_text 
 *   Secondary string of text.
 * @param string $picture 
 *   Icon to post 
 * @param string $link 
 *   URL for $main_text to link to.
 * @param array $actions 
 *   Array containing a single associative array, the keys of which must be 'name' and 'link'.
 * @param array $privacy 
 *   Associative array containing the key 'value', the value being one of Facebook's defined privacy modes.
 * @return array $response
 *   Array containing the response from the Facebook API.
 * @author bryn austin bellomy
 */
function facemelt_post_to_wall($main_text, $secondary_text, $picture = NULL, $link = NULL, $actions = array(), $privacy = array('value' => 'ALL_FRIENDS')) {
  if (!($facebook = facemelt_fb()) || !($me = facemelt_me())) {
    if (facemelt_debug_mode()) {
      facemelt_dsm(array('Facebook' => $facebook, 'me' => $me), 'Cannot post to wall -- not logged in.');
    }
    return FALSE;
  }
  $session = $facebook->getSession();
  if (!$session || !$session['access_token']) {
    if (facemelt_debug_mode()) {
      facemelt_dsm(array('Facebook session' => $session), 'Cannot post to wall -- session or access token not available.');
    }
    return FALSE;
  }
  
  $params = array(
    'name' => $main_text,
    'caption' => $secondary_text,
    'privacy' => $privacy,
    'link' => $link,
    'picture' => $picture,
    'actions' => $actions,
  );
  
  $response = $facebook->api('/me/feed', 'POST', $params);
  return $response;
}

/**
 * Get the app access token for your app.  This is necessary for
 * certain API calls like creating and destroying test users.
 *
 * @return void
 * @author bryn austin bellomy
 */
function facemelt_fb_get_app_access_token() {
  /*https://graph.facebook.com/oauth/access_token?
       client_id=YOUR_APP_ID&client_secret=YOUR_APP_SECRET&
       grant_type=client_credentials*/
       
  $facebook = facemelt_fb();
  $params = array(
    'client_id' => $facebook->getAppId(),
    'client_secret' => $facebook->getApiSecret(),
    'grant_type' => 'client_credentials',
  );
  $response = facemelt_fb_graph_request("/oauth/access_token", 'GET', $params, FALSE);
  if ($response and substr($response, 0, strlen('access_token=')) == 'access_token=') {
    return substr($response, strlen('access_token='));
  }
  return FALSE;
}

/**
 * Create a Facebook test user.
 *
 * @param bool $installed 
 *   Whether the user should be created having already authorized the app.
 * @param string $perms
 *   If $installed is TRUE, $perms should be used to list the permissions
 *   that the test user has granted to the app.
 * @return Array containing Facebook API response message.  This will
 *   contain the test user's unique login URL and some other data.
 * @author bryn austin bellomy
 */
function facemelt_fb_create_test_user($installed, $perms = NULL) {
  $facebook = facemelt_fb();
  $appId = $facebook->getAppId();
  if (!($app_access_token = facemelt_fb_get_app_access_token())) {
    return FALSE;
  }
  $params = array(
    'installed' => $installed,
    'access_token' => $app_access_token,
  );
  if ($perms) {
    $params['permissions'] = $perms;
  }
  $response = $facebook->api("/$appId/accounts/test-users", 'POST', $params);
  return $response;
}

/**
 * List all of the app's Facebook test users.
 *
 * @return void
 * @author bryn austin bellomy
 */
function facemelt_fb_show_test_users() {
  $facebook = facemelt_fb();
  $appId = $facebook->getAppId();
  if (!($app_access_token = facemelt_fb_get_app_access_token())) {
    return FALSE;
  }

  $params = array(
    'access_token' => $app_access_token,
  );
  $response = facemelt_fb_graph_request("/$appId/accounts/test-users", 'GET', $params);
  return $response;
}

/**
 * Rewrite of the Facebook PHP SDK's Graph API request function.  The only difference
 * is that the caller can specify whether or not json_decode should be called on the
 * results.  When json_decode is called on a plain string, it returns empty, so some
 * API responses will incorrectly appear blank; hence why the caller should be able
 * to choose.
 *
 * @param string $path 
 *   API request endpoint.
 * @param string $method 
 *   'GET' or 'POST'
 * @param array $params 
 *   Array containing various parameters for the request.
 * @param bool $json_decode
 *   TRUE/FALSE (whether or not to json_decode the response).
 * @return Facebook API response array.
 * @author bryn austin bellomy
 */
function facemelt_fb_graph_request($path, $method = 'GET', $params = array(), $json_decode = TRUE) {
  $facebook = facemelt_fb();
  
  $url = 'https://graph.facebook.com/';
  if ($path) {
    if ($path[0] === '/') {
      $path = substr($path, 1);
    }
    $url .= $path;
  }
  
  if (is_array($method) && empty($params)) {
    $params = $method;
    $method = 'GET';
  }
  $params['method'] = $method; // method override as we always do a POST

  if (!isset($params['access_token'])) {
    $params['access_token'] = $facebook->getAccessToken();
  }

  // json_encode all params values that are not strings
  foreach ($params as $key => $value) {
    if (!is_string($value)) {
      $params[$key] = json_encode($value);
    }
  }
  
  if (!$ch) {
    $ch = curl_init();
  }

  $opts = Facebook::$CURL_OPTS;
  if ($facebook->useFileUploadSupport()) {
    $opts[CURLOPT_POSTFIELDS] = $params;
  } else {
    $opts[CURLOPT_POSTFIELDS] = http_build_query($params, null, '&');
  }
  $opts[CURLOPT_URL] = $url;

  // disable the 'Expect: 100-continue' behaviour. This causes CURL to wait
  // for 2 seconds if the server does not support this header.
  if (isset($opts[CURLOPT_HTTPHEADER])) {
    $existing_headers = $opts[CURLOPT_HTTPHEADER];
    $existing_headers[] = 'Expect:';
    $opts[CURLOPT_HTTPHEADER] = $existing_headers;
  } else {
    $opts[CURLOPT_HTTPHEADER] = array('Expect:');
  }

  curl_setopt_array($ch, $opts);
  $result = curl_exec($ch);

  if (curl_errno($ch) == 60) { // CURLE_SSL_CACERT
    self::errorLog('Invalid or no certificate authority found, using bundled information');
    curl_setopt($ch, CURLOPT_CAINFO,
                dirname(__FILE__) . '/fb_ca_chain_bundle.crt');
    $result = curl_exec($ch);
  }

  if ($result === false) {
    $e = new FacebookApiException(array(
      'error_code' => curl_errno($ch),
      'error'      => array(
        'message' => curl_error($ch),
        'type'    => 'CurlException',
      ),
    ));
    curl_close($ch);
    throw $e;
  }
  curl_close($ch);
  
  if ($json_decode) {
    $result = json_decode($result, TRUE, 512);
  }

  // results are returned, errors are thrown
  if (is_array($result) && isset($result['error'])) {
    $e = new FacebookApiException($result);
    switch ($e->getType()) {
      // OAuth 2.0 Draft 00 style
      case 'OAuthException':
      // OAuth 2.0 Draft 10 style
      case 'invalid_token':
        $facebook->setSession(null);
    }
    throw $e;
  }
  return $result;
}




