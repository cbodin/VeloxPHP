<?php
/**
 * Velox settings class.
 *
 * Handles all settings for the framework and makes appropriate
 * changes when they are set.
 *
 * @package Velox
 * @since 1.0
 */
class Settings {

  /**
   * Contains all settings for the site.
   *
   * @var array
   */
  private static $settings = null;

  /**
   * Reads the application settings file.
   *
   * All methods call this function to make sure the settings file
   * has been read. 
   */
  private static function read() {
    if (self::$settings === null) {
      // Create an empty settings array and include the settings file.
      $settings = array();
      $file = VELOX_ROOT . '/application/settings.php';
      require_once $file;

      self::$settings = $settings;
    }
  }

  /**
   * Stores the application settings.
   */

  /**
   * Gets a specific setting.
   *
   * @param string $key
   * @param mixed $default optional
   *   Specify what the returned value should be if the specified key
   *   does not exist.
   *
   * @return mixed
   */
  static function get($key, $default = null) {
    self::read();

    if (isset(self::$settings[$key])) {
      return self::$settings[$key];
    }

    return $default;
  }

  /**
   * Sets a specific setting.
   *
   * @param string $key
   * @param mixed $value
   */
  static function set($key, $value) {
    self::read();

    self::$settings[$key] = $value;
  }

  /**
   * Deletes a specific setting.
   *
   * @param string $key
   */
  static function delete($key) {
    self::read();

    unset(self::$settings[$key]);
  }

}
