<?php

function facemelt_page_admin_view_caches() {
  $facemelt_caches = cache_money_get_cache_names('facemelt');

  $form = array('#tree' => TRUE);
  foreach ($facemelt_caches as $cache_name) {
    $cache_obj = cache_money_get_cached_data('facemelt', $cache_name);

    if ($cache_obj && $cache_obj->data) {
      $fn = (function_exists('kprint_r') ? 'kprint_r' : 'print_r');
      $contents = $fn($cache_obj->data, TRUE);
    }
    else $contents = '<font color="red"><strong>No data found.</strong></font>';
    
    $sanitized = preg_replace('/[\w:\?]/', '_', $cache);
    $form["cache_$sanitized"] = array(
      '#type' => 'fieldset',
      '#title' => $cache,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      'contents' => array('#value' => $contents),
    );
  }

  return $form;
}

