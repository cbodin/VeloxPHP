<?php
/**
 * Velox Facebook class.
 *
 * Handles facebook settings and contains the facebook instance.
 * 
 * @package VeloxFacebook
 */
class VeloxFacebook {

  /**
   * Holds the Facebook instance.
   *
   * @var null|Facebook
   */
  private static $instance = null;

  /**
   * Channel page callback, used by facebook js sdk for various reasons.
   *
   * @return string
   *   The required facebook channel page content.
   */
  static function channelPage() {
    $settings = self::getSettings();

    $cache_expire = 60*60*24*365;
    header("Pragma: public");
    header("Cache-Control: max-age=".$cache_expire);
    header('Expires: ' . gmdate('D, d M Y H:i:s', time()+$cache_expire) . ' GMT');

    return '<script src="//connect.facebook.net/' . $settings['locale'] . '/all.js"></script>';
  }

  /**
   * Getter for settings
   *
   * @return array
   */
  static function getSettings() {
    $settings = Settings::get('VeloxFacebook', array());

    // Add defaults
    $settings += array(
      'appId' => '',
      'secret' => '',
      'locale' => 'en_US',
    );

    return $settings;
  }

  /**
   * Instanciates and/or returns the facebook object.
   *
   * @return Facebook
   */
  static function getInstance() {
    if (self::$instance === null) {
      $settings = self::getSettings();

      if (!class_exists('Facebook')) {
        throw new Exception('Facebook library missing.');
      }

      self::$instance = new Facebook($settings);
    }

    return self::$instance;
  }

  /**
   * Checks if the user is liking the page.
   *
   * We can only be really sure during the first pageload inside the
   * facebook canvas, as we're losing the signed request on all
   * subsequent requests. So we will always return true if no signed
   * request is available.
   *
   * @return bool
   */
  function likesPage() {
    $facebook = self::getInstance();

    $signed_request = $facebook->getSignedRequest();

    if (!empty($signed_request['page']['liked']) && $signed_request['page']['liked'] == 1) {
      return true;
    }
    else if (isset($signed_request['page']['liked'])) {
      return false;
    }

    return true;
  }

}
