<?php

/**
 * facemelt_debug_mode()
 *
 * Clean way of letting various parts of the code check
 * whether they can display debug messages.
 *
 * @return void
 * @author bryn austin bellomy
 */
function facemelt_debug_mode($active = FALSE) {
  if (user_access('use facemelt debug mode') &&
      ((!$active || $_GET['debug'] == 1) || variable_get('facemelt_debug_mode', FALSE) == TRUE)) {
    return TRUE;
  }
  return FALSE;
}

/**
 * facemelt_dsm()
 *
 * dsm() wrapper to keep us from depending on devel and to encapsulate some
 * extra logic.
 *
 * @return void
 * @author bryn austin bellomy
 */
function facemelt_dsm($obj, $title = NULL) {
  if (facemelt_debug_mode()) {
    if (function_exists('dsm'))
      dsm($obj, $title);
    else
      drupal_set_message();
  }
}

/**
 * Wrapper for drupal_goto that always preserves $_GET variables (useful for IFRAMEd viewing).
 *
 * @param string $path 
 * @return void
 * @author bryn austin bellomy
 */
function facemelt_goto($path) {
  $query = (array)$_GET;
  unset($query['q']);
  drupal_goto($path, $query);
}

function facemelt_is_ajax_request() {
  if (isset($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == strtolower('XMLHttpRequest')) {
    return TRUE;
  }
  return FALSE;
}

function facemelt_is_cron() {
  if (strpos($_SERVER['REQUEST_URI'], 'cron.php') !== FALSE) {
    return TRUE;
  }
  return FALSE;
}


/**
 * watchdog() wrapper.
 *
 * @author bryn austin bellomy
 */
function facemelt_watchdog($message, $vars = array(), $severity = WATCHDOG_ERROR) {
  watchdog('facemelt', $message, $vars, $severity);
}

/**
 * watchdog() wrapper for handling PHP exceptions.
 *
 * @param string $exception_obj
 * @param string $function
 *   Simply use the PHP constant __FUNCTION__ here.
 * @param string $message
 * @param string $vars
 * @param string $severity
 * @author bryn austin bellomy
 */
function facemelt_watchdog_exception($exception_obj, $function, $message = NULL, $vars = NULL, $severity = WATCHDOG_ERROR) {
  $full_message = "Function '!function': Exception caught (code: !code).";
  if ($message) {
    $full_message .= "  Message: \"$message\"";
  }
  $full_message .= "  <pre>!exception</pre>";

  $vars = array(
    '!function' => $function,
    '!code' => $exception_obj->getCode(),
    '!exception' => print_r($exception_obj, TRUE),
  );
  facemelt_watchdog($full_message, $vars, $severity);
}

