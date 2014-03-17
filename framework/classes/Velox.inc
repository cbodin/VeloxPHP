<?php
/**
 * Velox class.
 *
 * Ties together the whole system.
 *
 * @package Velox
 * @since 1.0
 */
class Velox {

  /**
   * Stores the page title.
   *
   * @var string
   */
  private static $pageTitle;

  /**
   * The page title - name separator. This is used when setting a
   * title for a page or something other than the front page.
   *
   * With the title separator at "|" the title will look like
   * "About Velox PHP" when the site name is Velox PHP and the title
   * is set to "About".
   *
   * @var string
   */
  private static $pageTitleSeparator = ' | ';

  /**
   * Initializes the framework.
   *
   * Adds the enabled modules and looks for all the themes.
   */
  static function initialize() {
    // Add enabled modules
    Module::addModules();

    // Set the default title
    self::setTitle();

    // Call init actions
    Module::invokeAction('init');
  }

  /**
   * Outputs the site to the browser.
   */
  static function display() {
    // Check if we got a matching url route
    $page_info = Router::match();

    // Default output callback
    $output_callback = Response::NORMAL;

    // Let modules alter the matched route
    Module::alterAction('routeMatch', $page_info);

    // Check if a matching route was found. If not, output the 404
    // page.
    if ($page_info !== false) {
      // Get the current theme if we get a falsely return from the page
      // callback so we know which theme to output the 404-page in.
      $current_theme = Theme::getActive();

      // check if we explicitly set a theme for this page
      if (isset($page_info['theme'])) {
        Theme::setActive($page_info['theme']);
      }

      // Check if a file was specified, and include that file.
      // This can be used to have page callbacks in another file.
      if (isset($page_info['file'])) {
        include_once $page_info['file'];
      }

      // Callback arguments is an array with the value corresponding
      // to the index of the parts in the route. If you want the :num
      // from the url a/:num/c as the first argument to the callback,
      // the callback arguments should look like array(0). You can
      // also specify non-numeric values and they will be passed
      // directly as they are. This can be used to have one callback
      // for two slightly different routes.
      $arguments = array();
      foreach ($page_info['callback arguments'] as $argument) {
        if (is_int($argument)) {
          $arguments[] = Url::path($argument);
        }
        else {
          $arguments[] = $argument;
        }
      }

      self::ensureCallable($page_info['callback']);
      $content = call_user_func_array(
        $page_info['callback'],
        $arguments
      );

      // A falsey return will generate a 404 Page Not Found.
      if ($content === false) {
        $content = self::error404();

        Theme::setActive($current_theme);
      }
      else {
        // Set page title
        if (array_key_exists('title', $page_info)) {
          self::setTitle($page_info['title']);
        }

        $output_callback = $page_info['output callback'];
      }
    }
    else {
      $content = self::error404();
    }

    // Let modules alter the output before its being passed to the
    // output method.
    Module::alterAction('render', $content, $page_info);

    self::ensureCallable($output_callback);
    $output = call_user_func($output_callback, $content);

    // Finally let modules alter the content before its being
    // outputted to the page.
    Module::alterAction('display', $output, $page_info);

    print $output;
  }

  /**
   * Set the site title, if no arguments are passed it will use the
   * site name as the title.
   *
   * Passing null as $title and true for $append_name will remove
   * the page title altogether.
   *
   * @param string|null $title optional
   * @param bool $append_name optional
   *   Append the site name to the title.
   *
   * @return void
   */
  static function setTitle($title = null, $append_name = true) {
    if ($title === null) {
      $title = '';
    }

    if ($append_name && !empty($title)) {
      $title .= self::$pageTitleSeparator;
    }

    if ($append_name) {
      $title .= Settings::get('sitename', '');
    }

    self::$pageTitle = $title;
  }

  /**
   * Returns the current page title.
   *
   * @return string
   */
  static function getTitle() {
    return self::$pageTitle;
  }

  /**
   * Sets the title separator.
   *
   * @param string $separator
   */
  static function setTitleSeparator($separator) {
    self::$pageTitleSeparator = $separator;
  }

  /**
   * Gets the title separator.
   *
   * @return string
   */
  static function getTitleSeparator() {
    return self::$pageTitleSeparator;
  }

  /**
   * Ensures that a class callable is an array for PHP < 5.2.3
   */
  static function ensureCallable(&$callable) {
    // Return early if strings are allowed or if its already an array.
    if (VELOX_STATIC_METHOD_STRING_CALLABLE || is_array($callable)) {
      return;
    }

    $callable = explode('::', $callable);
  }

  /**
   * Error 404 page.
   *
   * @return string
   *   The content of the 404 page.
   */
  private static function error404() {
    header('HTTP/1.0 404 Not Found');

    $title = 'Page not found';
    $content = Theme::render('error-404');

    // Let modules alter the title and content for the 404 page.
    Module::alterAction('error404', $title, $content);

    self::setTitle($title);
    
    return $content;
  }

}
