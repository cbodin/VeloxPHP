<?php
/**
 * Velox Facebook version 4.x class.
 * 
 * @package VeloxFacebook
 */
class VeloxFacebook4 {

  /**
   * Initializes the facebook sdk.
   */
  static function init() {
    $settings = self::getSettings();

    Facebook\FacebookSession::setDefaultApplication($settings['appId'], $settings['secret']);
  }

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

}
