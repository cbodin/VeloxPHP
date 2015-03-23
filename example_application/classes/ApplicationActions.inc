<?php

class ApplicationActions {  
  
  /**
  * Run some init code here
  */
  static function init() {
    Application::iframe_test();
  }
  
  /**
  * Add the routes(urls for the application) here
  */
  static function routes() {
    $routes[':front'] = array(
      'callback' => 'Application::page_start',
      'output callback' => Response::NORMAL,
    );
    
    $routes[':alpha/:alpha'] = array(
      'callback' => 'Application::page_street',
      'callback arguments' => array(0, 1),
      'output callback' => Response::NORMAL,
    );

    return $routes;
  }
  
}
