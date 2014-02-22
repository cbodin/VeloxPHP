<?php
class SampleModuleActions {
  static function init() {
    // print 'Hej';
  }

  static function routes() {
    $routes[':front'] = array(
      'title' => 'lol',
      'callback' => array('SampleModule', 'frontPage'),
      'type' => Router::PAGE_OUTPUT_BLANK,
    );

    return $routes;
  }
}
