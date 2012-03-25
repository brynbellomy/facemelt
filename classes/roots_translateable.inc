<?php

interface roots_translateable {
  static function get_type();
  function get_translations($src_type = NULL);
  function get_data($field = NULL);
  function translate_field($field_name, $data);
}