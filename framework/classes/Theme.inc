<?php
/**
 * Velox theme class.
 *
 * Handles theme registry and interfaces with the template system.
 *
 * @package Velox
 * @since 1.0
 */
class Theme {

  /**
   * The currently active theme.
   *
   * @var string
   */
  private static $theme;

  /**
   * Stores theme's path and config.
   *
   * @var array
   */
  private static $registry = array();

  /**
   * All classes that should be set on the body tag.
   *
   * @var array
   */
  private static $bodyClasses = array();

  /**
   * Variables the should be outputed to either the html or page
   * template.
   *
   * @var array
   */
  private static $variables = array(
    'html' => array(),
    'page' => array(),
  );

  /**
   * Holds extra content to be added at around the content inside the
   * body tag.
   *
   * @var array
   */
  private static $page = array(
    'top' => array(),
    'bottom' => array(),
  );

  /**
   * Sets the current theme.
   *
   * @param string $theme
   *   The name of the theme.
   *
   * @throws Exception
   *   If the theme is not found.
   */
  static function setActive($theme) {
    self::$theme = $theme;
  }

  /**
   * Returns the current theme.
   *
   * @return string
   *   The current theme.
   */
  static function getActive() {
    if (!isset(self::$theme)) {
      self::$theme = Settings::get('theme');
    }

    return self::$theme;
  }

  /**
   * Returns the theme config.
   *
   * If $theme is not supplied it will default
   * to the currently active theme.
   *
   * @param string $theme optional
   *   The name of the theme.
   *
   * @throws Exception
   *   If the theme can't be found.
   *
   * @return array
   *   The theme configuration.
   */
  static function config($theme = null) {
    // Default to the currently active theme
    if ($theme === null) {
      $theme = self::getActive();
    }

    // Build the theme registry for the theme.
    if (!isset(self::$registry[$theme])) {
      self::buildConfig($theme);
    }

    // Make sure the theme exists
    if (self::$registry[$theme] === false) {
      throw new Exception('Missing theme: ' . $theme);
    }

    return self::$registry[$theme];
  }

  /**
   * Returns the path to the theme.
   *
   * If $theme is not supplied, it will default
   * to the currently active theme.
   *
   * @param string $theme
   *   The name of the theme.
   * @param bool $absolute
   *   Return an absolute path to the theme.
   *
   * @return string
   *   The path to the theme.
   */
  static function path($theme = null, $absolute = false) {
    $config = self::config($theme);

    if ($absolute) {
      return VELOX_ROOT . '/' . $config['path'];
    }

    return $config['path'];
  }

  /**
   * Renders a template file and returns the output.
   *
   * @param string $tmpl
   *   The name of the template (without the .tpl.php).
   * @param array $variables optional
   *   The variables that should be available in the template.
   * @param string $theme optional
   *   Render the template in another theme than the active.
   *
   * @return string
   *   The rendered template output.
   */
  static function render($tmpl, $variables = array(), $theme = null) {
    // Get the theme config
    $config = self::config($theme);

    // Call the render method of the themes selected template engine.
    $callable = array(
      $config['template_engine'],
      'render',
    );
    $args = array(
      $theme,
      $config['template_paths'],
      $tmpl,
      $variables,
    );

    Velox::ensureCallable($callable);
    return call_user_func_array($callable, $args);
  }

  /**
   * Set variable to a specific base template.
   *
   * @param string $template
   *   The template to add a variable to. (html or page)
   * @param string $variable
   *   The name of the variable to add.
   * @param mixed $value
   *   The value of the $variable.
   */
  static function setVariable($template, $variable, $value) {
    self::$variables[$template][$variable] = $value;
  }

  /**
   * Returns all previously set variables.
   *
   * @param $template
   *   The template file to get variables for. (html or page)
   *
   * @return array
   */
  static function getVariables($template) {
    return self::$variables[$template];
  }

  /**
   * Adds content either before or after the main content
   * inside the html template.
   *
   * @param string $position
   *   Either top or bottom.
   * @param string $key
   *   A unique key for this value.
   * @param string $value
   *   The content to add.
   */
  static function setPage($position, $key, $value) {
    self::$page[$position][$key] = $value;
  }

  /**
   * Returns the previously set page content.
   *
   * @param string $position
   *   Either top or bottom.
   *
   * @return array
   */
  static function getPage($position) {
    return self::$page[$position];
  }

  /**
   * Adds a class to the body.
   *
   * @param string $class
   *   The class to add to the body.
   */
  static function addBodyClass($class) {
    self::$bodyClasses[] = $class;
  }

  /**
   * Removes a class from the body.
   *
   * @param string $class
   *   The class to remove.
   */
  static function removeBodyClass($class) {
    foreach (self::$bodyClasses as $key => $value) {
      if ($value == $class) {
        unset(self::$bodyClasses[$key]);
      }
    }
  }

  /**
   * Returns all previously set body classes.
   *
   * @return array
   */
  static function getBodyClasses() {
    return self::$bodyClasses;
  }

  /**
   * Builds the theme config.
   *
   * @param string $theme
   *   The name of the theme.
   */
  private static function buildConfig($theme) {
    $path = self::getThemePath($theme);

    if ($path === false) {
      self::$registry[$theme] = false;
    }
    else {
      $config = self::readConfigFile($path);

      // Add defaults. The default parent should always be the velox
      // theme so we get templates for 404-pages etc.
      $config += array(
        'name' => $theme,
        'path' => $path,
        'js'   => array(),
        'css'  => array(),
        'parent' => $theme == 'velox' ? false : 'velox',
        'template_paths' => array(
          $path . '/templates' 
        ),
        'template_engine' => 'PHPTemplate',
      );

      if ($config['parent']) {
        self::addParentConfig($config);
      }

      self::$registry[$theme] = $config;
    }
  }

  /**
   * Finds the correct path of the theme, either in the framework
   * or the application folder.
   *
   * @param string $theme
   *   The name of the theme.
   *
   * @return boolean|string
   *   false if the theme was not found. The internal path to the
   *   theme if found.
   */
  private static function getThemePath($theme) {
    $paths = array(
      'framework/themes/' . $theme,
      'application/themes/' . $theme,
    );

    foreach ($paths as $path) {
      if (is_dir(VELOX_ROOT . '/' . $path)) {
        return $path;
      }
    }

    return false;
  }

  /**
   * Reads and parses the theme config file.
   *
   * @param string $path
   *   The internal path to the theme.
   */
  private static function readConfigFile($path) {
    $config_file = VELOX_ROOT . '/' . $path . '/theme.config.inc';
    $config = array();

    // We do not require a theme config file
    if (!is_file($config_file)) {
      return $config;
    }

    include $config_file;

    // Check if this is a subtheme
    if (isset($parent)) {
      $config['parent'] = $parent;
    }

    // Check if this theme is using another template engine
    if (isset($template_engine)) {
      $config['template_engine'] = $template_engine;
    }

    if (isset($js)) {
      foreach ($js as $script) {
        // check if its a internal or external js file
        if (!self::isExternal($script)) {
          $script = $path . '/' . $script;
        }

        $config['js'][] = $script;
      }
    }

    if (isset($css)) {
      foreach ($css as $style) {
        // If the style value is an array, this is a media
        // specific stylesheet.
        if (is_array($style)) {
          foreach ($style as $media => $media_style) {
            // check if its a internal or external css file
            if (!self::isExternal($media_style)) {
              $media_style = $path . '/' . $media_style;
            }
            
            $config['css'][][$media] = $media_style;
          }
        }
        else {
          // check if its a internal or external css file
          if (!self::isExternal($style)) {
            $style = $path . '/' . $style;
          }
          
          $config['css'][] = $style;
        }
      }
    }

    return $config;
  }

  /**
   * Check if the url is an external resource url. This method will
   * also allow for url's starting with // to be external as the
   * browser will automatically select protocol depending on what
   * protocol the site is currently at.
   * 
   * @param string $url
   * 
   * @return boolean
   */
  private static function isExternal($url) {
    // Do the substr check first as that is the fastest.
    if (substr($url, 0, 2) == '//') {
      return true;
    }
    elseif (filter_var($url, FILTER_VALIDATE_URL) !== false) {
      return true;
    }

    return false;
  }

  /**
   * Adds inherited js and css to a child theme from a parent theme.
   *
   * @param array $config
   *   The sub theme's config.
   */
  private static function addParentConfig(&$config) {
    // Fetch parent config
    $parent_config = self::config($config['parent']);

    // Append the js, css and template paths from the parent theme.
    // The order here is important as we want the parent themes js and
    // css first. However, the template paths should be in the
    // reversed order as templates in the child theme should always
    // preceed the parents.
    $config['js'] = array_merge(
      $parent_config['js'],
      $config['js']
    );
    $config['css'] = array_merge(
      $parent_config['css'],
      $config['css']
    );
    $config['template_paths'] = array_merge(
      $config['template_paths'],
      $parent_config['template_paths']
    );
  }

}
