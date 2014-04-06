<?php
/**
 * Velox request class
 *
 * Handles $_GET, $_POST and $_COOKIE cleanup from magic quotes
 * without altering the original variables.
 *
 * @package Velox
 * @since 1.0
 */
class Request {

  /**
   * Stores the check if magic quotes is enabled.
   *
   * @var boolean
   */
  private static $magicQuotes;

  /**
   * Stores the check if magic quotes sybase is enabled.
   *
   * @var boolean
   */
  private static $magicQuotesSybase = true;

  /**
   * Contains cleaned $_GET variables.
   *
   * @var array
   */
  private static $get = array();

  /**
   * Contains cleaned $_POST variables.
   *
   * @var array
   */
  private static $post = array();

  /**
   * Contains cleaned $_COOKIE variables.
   *
   * @var array
   */
  private static $cookie = array();

  /**
   * Fetches get variables.
   *
   * @param string|null $key optional
   *   The get key to fetch. If omitted or null the whole get array
   *   will be returned.
   * @param mixed $default optional
   *   The default value to get when the key doesn't exist.
   *   Default: null
   *
   * @return mixed
   */
  static function get($key = null, $default = null) {
    if ($key === null) {
      return self::ensureClean($_GET);
    }

    if (!isset($_GET[$key])) {
      return $default;
    }

    // Check if we got a cleaned value stored.
    if (!isset(self::$get[$key])) {
      self::$get[$key] = self::ensureClean($_GET[$key]);
    }

    return self::$get[$key];
  }

  /**
   * Fetches post variables.
   *
   * @param string|null $key optional
   *   The post key to fetch. If omitted or null the whole post array
   *   will be returned.
   * @param mixed $default optional
   *   The default value to get when the key doesn't exist.
   *   Default: null
   *
   * @return mixed
   */
  static function post($key = null, $default = null) {
    if ($key === null) {
      return self::ensureClean($_POST);
    }

    if (!isset($_POST[$key])) {
      return $default;
    }

    // Check if we got a cleaned value stored.
    if (!isset(self::$post[$key])) {
      self::$post[$key] = self::ensureClean($_POST[$key]);
    }

    return self::$post[$key];
  }

  /**
   * Fetches cookie variables.
   *
   * @param string|null $key optional
   *   The cookie key to fetch. If omitted or null all cookies will
   *   be returned.
   * @param mixed $default optional
   *   The default value to get when the key doesn't exist.
   *   Default: null
   *
   * @return mixed
   */
  static function cookie($key = null, $default = null) {
    if ($key == null) {
      return self::ensureClean($_COOKIE);
    }

    if (!isset($_COOKIE[$key])) {
      return $default;
    }

    // Check if we got a cleaned value stored.
    if (!isset(self::$cookie[$key])) {
      self::$cookie[$key] = self::ensureClean($_COOKIE[$key]);
    }

    return self::$cookie[$key];
  }

  /**
   * Gets the current request method. Either 'POST' or 'GET'.
   *
   * @return string
   */
  static function method() {
    return $_SERVER['REQUEST_METHOD'];
  }

  /**
   * Ensures that the variable has been cleaned of magic quotes.
   *
   * @param mixed $value
   *   The value to ensure is clean.
   *
   * @return mixed
   *   The value that is ensured to be clean of magic quotes.
   */
  private static function ensureClean($value) {
    if (self::magicQuotes()) {
      self::cleanDeep($value);
    }

    return $value;
  }

  /**
   * Cleans the value recursively.
   *
   * @param mixed $value
   *   The value to clean
   */
  private static function cleanDeep(&$value) {
    if (is_string($value)) {
      if (self::$magicQuotesSybase) {
        $value = str_replace("''", "'", $value);
      }
      else {
        $value = stripslashes($value);
      }
    }
    elseif (is_array($value)) {
      foreach ($value as &$v) {
        self::cleanDeep($v);
      }
    }
  }

  /**
   * Checks if magic quotes is on.
   *
   * @return boolean
   */
  private static function magicQuotes() {
    if (!isset(self::$magicQuotes)) {
      if (!function_exists('get_magic_quotes_gpc')) {
        self::$magicQuotes = false;
      }
      elseif (!get_magic_quotes_gpc()) {
        self::$magicQuotes = false;
      }
      else {
        $quotes_sybase = strtolower(ini_get('magic_quotes_sybase'));

        if (empty($quotes_sybase) || $quotes_sybase === 'off') {
          self::$magicQuotesSybase = false;
        }

        self::$magicQuotes = true;
      }
    }

    return self::$magicQuotes;
  }

}
