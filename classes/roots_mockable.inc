<?php

interface roots_mockable {
  function generate_mock_object();
  function generate_mock_data($field);
  static function generate_mock_objects($num);
}