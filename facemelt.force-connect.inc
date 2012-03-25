<?php

function facemelt_access__force_fb_connect_sometimes() {
  return variable_get('facemelt_force_fb_connect_sometimes', FALSE);
}

function facemelt_access__force_fb_connect_paths() {
  return variable_get('facemelt_force_fb_connect_paths', array());
}

function facemelt_access__forced_to_connect_message() {
  return variable_get('facemelt_forced_to_connect_message', NULL);
}

function facemelt_access__forced_to_connect_redirect_url() {
  return variable_get('facemelt_forced_to_connect_redirect_url', NULL);
}

function facemelt_access__failed_path_test() {
  $paths = facemelt_access__force_fb_connect_paths();
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


