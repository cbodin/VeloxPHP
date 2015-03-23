<?php

class Application {  
  
  /**
  * Maybe add some properties here?!?
  */
  private static $iframe = null;
  
  /**
  * Page callback for the start page
  */
  static function page_start() {    
    Theme::addBodyClass('start');
    return Theme::render('page-start');
  }
  
  /**
  * Page callback for the street page
  */
  static function page_street($city, $street) {
    print "<br /><br /><br /><pre>"; print_r($city); print "</pre>";
    print "<br /><br /><br /><pre>"; print_r($street); print "</pre>";
    
    Theme::addBodyClass('street');
    return Theme::render('page-street');
  }    
  
  /**
  * Custom method to check if page run in iframe(simple add a ?iframe=true to the iframe src url)
  */
  static function iframe_test() {
    if (isset($_GET['iframe']) && $_GET['iframe'] == 'true') {
      self::$iframe = true;
      Theme::addBodyClass('iframe');
    }
  }
  
}
