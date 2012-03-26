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
 * Return the global Facebook SDK object.  Initialize it first if needed.
 *
 * @return object
 * @author bryn austin bellomy
 */
function &facemelt_fb($require_session = TRUE) {
  /*static $drupal_static_fast;
  if (!isset($drupal_static_fast))
    $drupal_static_fast['facebook'] = &drupal_static(__FUNCTION__);
  $facebook = &$drupal_static_fast['facebook'];*/
  static $facebook;
  $facebook = &drupal_static('facemelt_fb');
  
  if (!isset($facebook))
    facemelt_start_facebook($require_session);

  return $facebook;
}

/**
 * Return a Facebook object representing the currently logged-in user.
 *
 * @return object
 * @author bryn austin bellomy
 */
function &facemelt_me($require_session = TRUE) {
  static $drupal_static_fast;
  if (!isset($drupal_static_fast))
    $drupal_static_fast['me'] = &drupal_static(__FUNCTION__);
  $me = &$drupal_static_fast['me'];
  
  if (!isset($me))
    facemelt_fb($require_session);

  return $me;
}



/**
 * Initialize the Facebook API object and set up the SESSION cache.
 *
 * @return TRUE if successfully initialized, FALSE otherwise.
 * @author bryn austin bellomy
 */
function facemelt_start_facebook($require_session = TRUE) {

  facemelt_require_php_sdk();

  // make sure the pointers we're going to fill are pointing at the right static
  // caches (drupal_static_fast pattern iz r weird)
  // http://api.drupal.org/api/drupal/includes%21bootstrap.inc/function/drupal_static/7
  //$drupal_static_fast['facebook'] = &drupal_static('facemelt_fb');
  $drupal_static_fast['me'] = &drupal_static('facemelt_me');
  //$facebook = &$drupal_static_fast['facebook'];
  $facebook = &drupal_static('facemelt_fb');
  $me       = &$drupal_static_fast['me'];

  // create the application instance
  $facebook = new facemelt_facebook(array(
    'appId'  => facemelt_facebook_app_id(),
    'secret' => facemelt_facebook_app_secret(),
    'cookie' => TRUE,
  ));
  
  // @@DEBUG: $me = NULL;
  $_me = NULL;
  $session = $facebook->getUser();

  // @@DEBUG: maybe this will accommodate the fboauth module?
  //if (!$session && !$require_session)
  //  return;

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
      if (!$_me) {
        $me = NULL;
        $_SESSION[cache_money_get_cache_money_prefix()] = array();
        unset($_SESSION['registered-email-address']);
      }
      return TRUE;
    } catch (FacebookApiException $e) {
      // if the user isn't logged in to facebook, clear the entries in the SESSION cache that are facebook-dependent (my friends, etc)
      if (!$me) {
        $_SESSION[cache_money_get_cache_money_prefix()] = array();
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

function facemelt_oauth_dialog_settings($action, $opts = array()) {
  $choose = function(&$a, $b) { $a = ($a ? $a : $b); }; // <3 lambdas
  
  $choose(&$opts['redirect_uri'], facemelt_fbconnect_redirect_path(TRUE));
  $choose(&$opts['scope'], facemelt_oauth_scope($action));
  $choose(&$opts['channel_url'], facemelt_fb_channel_url());
  $choose(&$opts['display'], 'popup');
  
  return $opts;
}

function facemelt_oauth_dialog_settings_javascript($action = 'connect', $opts = array()) {
  $opts = facemelt_oauth_dialog_settings($action, $opts);


  $to_return = <<<EOL
    var Facemelt = Facemelt || {};
    Facemelt.actions = Facemelt.actions || {};
    Facemelt.actions.$action = Facemelt.actions.$action || {};
    Facemelt.actions.$action.display = '{$opts['display']}';
    Facemelt.actions.$action.redirect_uri = '{$opts['redirect_uri']}';
    Facemelt.actions.$action.scope = '{$opts['scope']}';
    Facemelt.actions.$action.response_type = '{$opts['response_type']}';
EOL;
  return $to_return;
}

function facemelt_oauth_dialog_url($action = 'connect', $opts = array()) {
  $opts = facemelt_oauth_dialog_settings($action, $opts);

  return "https://www.facebook.com/dialog/oauth/?"
    . "client_id={$opts['app_id']}"
    . "&redirect_uri={$opts['redirect_uri']}"
    . "&state={$opts['state']}"
    . "&scope={$opts['scope']}"
    . "&display={$opts['display']}";
}

function facemelt_get_facebook_api_root_markup($act_on_statusChange = TRUE) {
  $on_connect = NULL;
  if ($act_on_statusChange)
    $on_connect = "window.location.replace('/');";

  $js = <<<EOL
  <div id="fb-root"></div>
  <script type="text/javascript">
  window.fbAsyncInit = function() {
    FB.init({
        appId      : Facemelt.facebookAppId,
        status     : true, 
        cookie     : true,
        xfbml      : true,
        oauth      : true,
        channelUrl : Facemelt.facebookChannelURL.replace(/^[a-zA-Z0-9]+:/, '')
    });

    if (!Facemelt.userLoggedIn) {
      FB.Event.subscribe('auth.login', function(response) {
        //alert('auth.login' + response.status);
        $on_connect
      });

      FB.Event.subscribe('auth.statusChange', function(response) {
        console.log('>> auth.statusChange', response);
        if (response.authResponse && response.status == 'connected') {
          //$on_connect
        }
      });
    }
  };

  (function(d){
    var js, id = 'facebook-jssdk'; if (d.getElementById(id)) {return;}
      js = d.createElement('script'); js.id = id; js.async = true;
    js.src = "//connect.facebook.net/en_US/all.js";
    d.getElementsByTagName('head')[0].appendChild(js);
  }(document));

  
  </script>
EOL;
  return $js;
}


/**
 * Redirect the user to the path given by facemelt_fbconnect_redirect_path().
 * Used for server-side FB connect flow control.
 */
function facemelt_fbconnect_redirect_after_connect() {
  //$path = facemelt_fbconnect_redirect_path();
  $path = facemelt_fbconnect_receive_authenticated_session_path(FALSE);
  if (is_string($path))
    facemelt_goto($path);
}

function facemelt_fbconnect_must_be_connected_at_current_path() {
  $paths = facemelt_fbconnect_paths_requiring_connect();
  if (is_array($paths)) {
    $q = drupal_get_normal_path($_GET['q']);
    $failed = 'mu';
    foreach ($paths as $path) {
      if (strpos($path, '~') === 0) { // negate flag
        if (trim(substr($path, 1)) == $q) {
          $failed = TRUE;
          break;
        }
      }
    }
    if ($failed === TRUE)
      return TRUE;
  }
  return FALSE;
}


/**
 * Try to perform a connect request against the Facebook Graph API via the means
 * available (basically just the fboauth module at the moment).
 *
 * @author bryn austin bellomy
 */
function facemelt_fb_connect() {
  if (facemelt_fboauth_enabled()) {
    module_load_include('inc', 'fboauth', 'includes/fboauth.fboauth');
    return fboauth_action_connect(
      facemelt_fb()->getAppId(),
      facemelt_fb()->getAccessToken());
  }
}

/**
 * Get the oauth scope for a given action.
 *
 * @author bryn austin bellomy
 */
function facemelt_oauth_scope($action = 'connect', $as_string = TRUE) {
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['oauth_scope'] = &drupal_static(__FUNCTION__);
  }
  $oauth_scope = &$drupal_static_fast['oauth_scope'];
  if (!isset($oauth_scope)) {
    $oauth_scope = module_invoke_all('facemelt_oauth_scope');
    drupal_alter('facemelt_oauth_scope', $oauth_scope);
  }
  $to_return = $oauth_scope[$action];
  if ($as_string) $to_return = implode(',', $to_return);
  return $to_return;
}


/**
 * Check if the fboauth module is installed and enabled.
 *
 * @author bryn austin bellomy
 */
function facemelt_fboauth_enabled() {
  static $enabled;
  if (!isset($enabled)) {
    $enabled = &drupal_static(__FUNCTION__);
    $enabled = module_exists('fboauth');
  }
  return $enabled;
}




/**
 * Facebook OAuth callback for initiating a Facebook connection.
 *
 * ...ripped straight of the fboauth module.
 */
function facemelt_handle_incoming_fb_user() {
  global $user;

  module_load_include('module', 'fboauth');
  module_load_include('inc', 'fboauth', 'includes/fboauth.fboauth');

  $fbuser = facemelt_me();
  $uid = fboauth_uid_load($fbuser->id);
  // If the user has logged in before, load their account and login.
  if (!$user->uid && $uid && ($account = user_load($uid))) {
    fboauth_login_user($account);
  }
  // If the Facebook e-mail address matches an existing account, bind them
  // together and log in as that account.
  elseif (!empty($fbuser->email) && ($account = array_shift(user_load_multiple(array(), array('mail' => $fbuser->email))))) {
    fboauth_save($account->uid, $fbuser->id);

    // Logins will be denied if the user's account is blocked.
    if (fboauth_login_user($account)) {
      drupal_set_message(t('You\'ve connected your account with Facebook.'));
    }
  }
  else {
    // If the user is already logged in, associate the two accounts.
    if ($user->uid) {
      fboauth_save($user->uid, $fbuser->id);
      drupal_set_message(t('You\'ve connected your account with Facebook.'));
    }
    // Register a new user only if allowed.
    elseif (variable_get('user_register', 1)) {
      $account = fboauth_create_user($fbuser);
      // Load the account fresh just to have a fully-loaded object.
      $account = user_load($account->uid);

      // If the account requires administrator approval the new account will
      // have a status of '0' and not be activated yet.
      if ($account->status == 0) {
        _user_mail_notify('register_pending_approval', $account);
        drupal_set_message(t('An account has been created for you on @sitename but an administrator needs to approve your account. In the meantime, a welcome message with further instructions has been sent to your e-mail address.', array('@sitename' => variable_get('site_name', ''))));
      }
      // Log in the user if no approval is required.
      elseif (fboauth_login_user($account)) {
        drupal_set_message(t('Welcome to @sitename. Basic information has been imported from Facebook into your account. You may want to <a href="!edit">edit your account</a> to confirm the details and set a password.', array('@sitename' => variable_get('site_name', ''), '!edit' => url('user/' . $account->uid . '/edit'))));
      }
      // If the login fails, fboauth_login_user() throws its own error message.
    }
    // Since user's can't create new accounts on their own, show an error.
    else {
      drupal_set_message('Your Facebook e-mail address does not match any existing accounts. If you have an account, you must first log in before you can connect your account to Facebook. Creation of new accounts on this site is disabled.');
    }
  }
}

