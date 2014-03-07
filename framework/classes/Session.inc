<?php
/**
 * Velox session handler.
 *
 * Simple wrapper to extend the session functionality.
 *
 * @package Velox
 * @since 1.0
 */
class Session {

  /**
   * Sets a session key
   *
   * @param string $key
   *   The $_SESSION key to set.
   * @param mixed $value
   *   The value to set.
   */
  public static function set($key, $value) {
    $_SESSION[$key] = $value;
  }

  /**
   * Gets a specific session value.
   *
   * @param string $key
   *   The $_SESSION key to get.
   * @param mixed $default optional
   *   The value to return if the session is not set
   *
   * @return mixed|null
   */
  public static function get($key, $default = null) {
    if (isset($_SESSION[$key])) {
      return $_SESSION[$key];
    }

    return $default;
  }

  /**
   * Unset's a specific session key.
   *
   * @param string $key
   *   The key to remove
   */
  public static function del($key) {
    unset($_SESSION[$key]);
  }

}
