<?php

/**
 * Implements hook_block_info().
 */
function facemelt_block_info() {
  $blocks['fb_root'] = array(
    'info' => t('Facebook API javascript'),
    'cache' => DRUPAL_CACHE_GLOBAL,
    'region' => '',
    'status' => 1,
  );

  return $blocks;
}

/**
 * Implements hook_block_view().
 */
function facemelt_block_view($delta = '') {
  $block = array();

  switch ($delta) {
    case 'fb_root':
      $block['content'] = facemelt_get_facebook_api_root_markup();
      break;

    default:
      break;
  }
  return $block;
}

