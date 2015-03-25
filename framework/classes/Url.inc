<?php
/**
 * Velox url class.
 *
 * Contains alot of useful methods related to Url generation etc.
 *
 * @package Velox
 * @since 1.0
 */
class Url {

  /**
   * Holds the unsecure and secure url.
   *
   * @var array
   */
  private static $urls;

  /**
   * Holds the currently requested path.
   *
   * @var string
   */
  private static $path;

  /**
   * Generates a url.
   *
   * Generates an url to the site root if the path parameter is
   * omitted or null, if not, generates an url to the specified path
   * according to the options passed 
   *
   *
   * @param string $path
   *   The path to get a url for, relative to the site root.
   * @param array $options optional
   *   An array with any of the following:
   *
   *   absolute: Set it to true to generate a absolute url.
   *             Default: false
   *
   *   query:    An array containing key => value, used to build a
   *             query.
   *             Default: none
   *
   *   fragment: Adds a fragment to the end of url (eg. '#frag'). Do
   *             not include the '#'.
   *             Default: none
   *
   *   scheme:   The scheme for this url (http or https)
   *             Default: https if we're on a secure connection, http
   *             otherwise.
   *
   *   clean:    If this url should be a clean url (without the
   *             index.php in the url).
   *             Default: Same as 'clean_urls' in the application
   *             settings file, or true if thats not defined.
   *
   * @return string
   *   The url to the site root or the the chosen path.
   */
  static function generate($path = null, $options = array()) {
    // Add defaults
    $options += array(
      'absolute' => false,
      'query'    => array(),
      'fragment' => null,
      'scheme'   => self::https() ? 'https' : 'http',
      'clean'    => Settings::get('clean_urls', true),
    );

    // Let modules alter the options and path before generating the
    // url.
    Module::alterAction('preUrlGenerate', $path, $options);

    $site_urls = self::get();

    if ($options['scheme'] == 'http') {
      $url = $site_urls['unsecure'];
    }
    else {
      $url = $site_urls['secure'];
    }

    $url .= '/';

    if (!$options['absolute']) {
      // Remove any slash we might have in the site directory.
      $directory = trim(parse_url($url, PHP_URL_PATH), '/');

      $url = '/';

      if (!empty($directory)) {
        $url .= $directory . '/';
      }
    }

    // Check if this is not a url to the front page.
    if (!empty($path) && $path != '/') {
      // If clean urls are disabled, add index.php to the url.
      // Check if this is a url to a static file as we cannot add
      // index.php to the url in that case.
      if (
        !$options['clean'] &&
        !file_exists(VELOX_ROOT . '/' . $path)
      ) {
        $url .= 'index.php/';
      }

      $url .= $path;
    }

    if (!empty($options['query'])) {
      $url .= '?' . http_build_query($options['query']);
    }

    if (!empty($options['fragment'])) {
      $url .= '#' . $options['fragment'];
    }

    $action_info = array(
      'path' => $path,
      'options' => $options,
    );

    // Let modules alter the url before output
    Module::alterAction('postUrlGenerate', $url, $action_info);

    return $url;
  }

  /**
   * Generates a url to the currently active theme.
   *
   * @param string $path
   *   The path to get a url for, relative to the theme directory.
   * @param array $options optional
   *   Passed directly to self::generate() +
   *   'theme': Specify another theme than the currently active
   *
   * @return string
   *   The url to the theme or to the chosen path relative to the
   *   theme.
   */
  static function generateTheme($path = null, $options = array()) {
    $theme = isset($options['theme']) ? $options['theme'] : null;
    $theme_path = Theme::path($theme);
    unset($options['theme']);

    if ($path !== null) {
      $theme_path .= '/' . $path;
    }

    return self::generate($theme_path, $options);
  }

  /**
   * Returns the url we're currently on.
   *
   * @param array $options optional
   *   An array with any of the following:
   *
   *   strip_query: Removes the query portion of the url.
   *                Default: true
   *
   *   absolute:    Returns an absolute url.
   *                Default: true
   *
   * @return string
   *   The current url
   */
  static function current($options = array()) {
    // Add defaults
    $options += array(
      'strip_query' => true,
      'absolute'    => true,
    );

    if (!$options['strip_query']) {
      $options['query'] = $_GET;
    }

    unset($options['strip_query']);

    return self::generate(self::path(), $options);
  }

  /**
   * Returns the current path.
   *
   * @param int $index optional
   *   Returns a specific part of the path.
   *   Default: none
   * @param bool $as_array optional
   *   Returns an array with all the parts instead of them imploded
   *   into a string.
   *   Default: false
   *
   * @return bool|string|array
   *   If an index is supplied, it will return the path part for that
   *   index. If that index isn't found it returns boolean false.
   *   If no index is defined and $as_array is false, it will return
   *   the current requested path as a string. If $as_array is true
   *   it will return all parts in an numerical array.
   */
  static function path($index = null, $as_array = false) {
    if (!isset(self::$path)) {
      $url = self::generate();
      $directory = trim(parse_url($url, PHP_URL_PATH), '/');

      // Use the REQUEST_URI as PATH_INFO is only available when
      // the index.php file is called directly instead of via a
      // rewrite rule.
      $path = $_SERVER['REQUEST_URI'];

      // Check if we're in a subdirectory and then remove the
      // directory to get the requested path.
      if (!empty($directory)) {
        $path = substr($path, strlen($directory) + 1);
      }

      // If we're not using clean urls, index.php will be in the
      // REQUEST_URI.
      if (strpos($path, '/index.php') === 0) {
        $path = substr($path, 10);
      }

      // Remove query string, urldecode and remove slashes
      self::$path = trim(urldecode(strtok($path, '?')), '/');
    }

    if ($index !== null) {
      $parts = explode('/', self::$path);

      if (isset($parts[$index])) {
        return $parts[$index];
      }

      return false;
    }

    if ($as_array) {
      return explode('/', self::$path);
    }

    return self::$path;
  }

  /**
   * Checks if we are on the front page. This is determined by
   * checking if the requested path is empty.
   *
   * @return bool
   */
  static function isFront() {
    $path = self::path();
    return strcmp($path, '') === 0;
  }

  /**
   * Redirects the browser.
   *
   * @param string $url
   *   The url to redirect to. If this is not an absolute url
   *   (starts with 'http' or 'https') it will generate a internal
   *   url using the Url::generate method.
   * @param array $options optional
   *   An options array to pass onto the Url::generate method if an
   *   internal url is being generated. In addition the following
   *   options is also available:
   *
   *   response_code: The http response code to use for the redirect
   *                  Default: 302
   */
  static function redirect($url, $options = array()) {
    // Add defaults
    $options += array(
      'response_code' => 302,
    );

    if (stripos($url, 'http') !== 0) {
      // Override 'absolute' as some web browsers only accept
      // absolute urls.
      $options['absolute'] = true;

      $url = self::generate($url, $options);
    }

    header('Location: ' . $url, true, $options['response_code']);
    exit;
  }

  /**
   * Reloads the current page.
   *
   * @param array $options optional
   *   An array with any of the following keys:
   *
   *   response_code: The http response code to use for the reload.
   *                  Default: 302
   *
   *   strip_query:   Remove the query portion from url before doing
   *                  a reload.
   *                  Default: false
   */
  static function reload($options = array()) {
    // Add defaults
    $options += array(
      'response_code' => 302,
      'strip_query'   => false,
    );

    $url = self::current(array(
      'strip_query' => $options['strip_query'],
    ));

    self::redirect($url, array(
      'response_code' => $options['response_code'],
    ));
  }

  /**
   * Check if we are currently on https.
   *
   * @return bool
   *   true if we're on https, or false if not.
   */
  static function https() {
    return (
      isset($_SERVER['HTTPS']) &&
      strtolower($_SERVER['HTTPS']) == 'on'
    );
  }

  /**
   * Figures out and returns the unsecure and secure site url.
   *
   * @return array
   *   An array containing the keys 'unsecure' and 'secure'.
   */
  static function get() {
    // Check if the data has already been cached.
    if (!isset(self::$urls)) {
      // Check if the site urls are set in the settings file.
      $site_urls = Settings::get('site_url');

      if (!empty($site_urls['unsecure'])) {
        self::$urls['unsecure'] = $site_urls['unsecure'];
      }
      elseif (self::https()) {
        if (isset($site_urls['secure'])) {
          $url = $site_urls['secure'];
        }
        else {
          $url = self::getUrlFromGlobals();
        }
        
        // Remove the s in https
        $url = substr_replace($url, '', 4, 1);

        self::$urls['unsecure'] = $url;
      }
      else {
        self::$urls['unsecure'] = self::getUrlFromGlobals();
      }

      if (isset($site_urls['secure'])) {
        self::$urls['secure'] = $site_urls['secure'];
      }
      else {
        // Add the s after the http in the unsecure url.
        $url = substr_replace(self::$urls['unsecure'], 's', 4, 0);

        self::$urls['secure'] = $url;
      }

      Settings::set('site_url', self::$urls);
    }

    return self::$urls;
  }

  /**
   * Creates and returns the site url from the $_SERVER array.
   *
   * @return string
   */
  private static function getUrlFromGlobals() {
    // Get any subdirectory the script might be running inside.
    $directory = trim(dirname($_SERVER['SCRIPT_NAME']), '/');

    // print $directory;

    // Get the root directory right on Windows
    if ($directory === '\\') {
      $directory = '';
    }

    $port = $_SERVER['SERVER_PORT'];
    $scheme = self::https() ? 'https' : 'http';

    // Create the url
    $url = $scheme . '://' . $_SERVER['HTTP_HOST'];

    // Check if this is the default port for the scheme
    if (
      ($scheme == 'https' && $port != 443) ||
      ($scheme == 'http' && $port != 80)
    ) {
      $url .= ':' . $port;
    }

    if (!empty($directory)) {
      $url .= '/' . $directory;
    }

    return $url;
  }

}
