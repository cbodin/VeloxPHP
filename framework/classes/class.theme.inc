<?php
/**
 * Velox theme class.
 *
 * Handles theme registry and interfaces with the template system.
 *
 * @package Velox
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
   * Variables the should be outputed to either the html or page template.
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
   * Initialization, sets up theme registry and sets the default theme.
   */
  static function initialize() {
    self::findThemes();
    self::parseConfigs();
    self::parseSubthemes();

    $theme = Settings::get('theme');
    self::setActive($theme);
  }

  /**
   * Sets the current theme.
   *
   * @param string $theme
   *   The name of the theme.
   */
  static function setActive($theme) {
    // Make sure the theme exists
    if (!isset(self::$registry[$theme])) {
      throw new Exception('Missing theme: ' . $theme);
    }

    self::$theme = $theme;
  }

  /**
   * Returns the current theme.
   *
   * @return string
   *   The current theme.
   */
  static function getActive() {
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
   */
  static function config($theme = null) {
    // Default to the currently active theme
    if ($theme === null) {
      $theme = self::$theme;
    }

    // Make sure the theme exists
    if (!isset(self::$registry[$theme])) {
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
    $theme_config = self::config($theme);

    if ($absolute) {
      return VELOX_ROOT . '/' . $theme_config['path'];
    }

    return $theme_config['path'];
  }

  /**
   * Renders a template file and returns the output.
   *
   * @param string $template
   *   The name of the template (without the .tpl.php).
   * @param array $variables optional
   *   The variables that should be available in the template.
   * @param string $theme optional
   *   Render the template in another theme than the active.
   *
   * @return string
   *   The rendered template output.
   */
  static function render($template, $variables = array(), $theme = null) {
    // Default to the active theme
    if ($theme === null) {
      $theme = self::getActive();
    }

    // Get the theme config
    $theme_config = self::config($theme);

    // Call the render method of the themes selected template engine.
    $callable = array(
      $theme_config['template_engine'],
      'render',
    );
    $args = array(
      $theme,
      $theme_config['template_paths'],
      $template,
      $variables,
    );

    return call_user_func_array($callable, $args);
  }

  /**
   * Set variable to a specific base template.
   *
   * @param string $template
   *   The template to add a variable to (html or page).
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
   * @return array
   */
  static function getVariables($template) {
    return self::$variables[$template];
  }

  /**
   * Adds content either before or efter the main content
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
   * Sets up the theme registry.
   */
  private static function findThemes() {
    // Search for themes.
    $core = glob(VELOX_ROOT . '/framework/themes/*', GLOB_ONLYDIR);
    $app = glob(VELOX_ROOT . '/application/themes/*', GLOB_ONLYDIR);

    // Add the app themes last, so they will overwrite core themes if
    // provided with the same theme name.
    $themes = array_merge($core, $app);

    // Get theme name and cleanup path.
    foreach ($themes as $theme) {
      $theme_name = basename($theme);

      $root_length = strlen(VELOX_ROOT . '/');
      $theme_path = substr_replace($theme, '', 0, $root_length);

      self::$registry[$theme_name] = array(
        'name' => $theme_name,
        'path' => $theme_path,
        'js' => array(),
        'css' => array(),
        'parent' => false,
        'template_paths' => array(
          $theme_path . '/templates' 
        ),
      );
    }
  }

  /**
   * Parses the theme configs.
   */
  private static function parseConfigs() {
    foreach (self::$registry as &$theme_config) {
      $config_file = VELOX_ROOT . '/' . $theme_config['path'] . '/theme.config.inc';

      // We do not *require* a theme config file
      if (!is_file($config_file)) {
        continue;
      }

      // Unset variables from previous loop
      unset($parent, $js, $css, $template_engine);

      include $config_file;

      // Check if this is a subtheme and parse the parent themes config
      if (isset($parent)) {
        $theme_config['parent'] = $parent;
      }

      // Check if this theme is using another template engine
      if (isset($template_engine)) {
        $theme_config['template_engine'] = $template_engine;
      }
      else {
        // Default to the built in engine.
        $theme_config['template_engine'] = 'PHPTemplate';
      }

      if (isset($js)) {
        foreach ($js as $script) {
          // check if its a internal or external js file
          if (filter_var($script, FILTER_VALIDATE_URL) === false && substr($script, 0, 2) != '//') {
            $theme_config['js'][] = $theme_config['path'] . '/' . $script;
          }
          else {
            $theme_config['js'][] = $script;
          }
        }
      }

      if (isset($css)) {
        foreach ($css as $style) {
          // If the style value is an array, this is a media
          // specific stylesheet.
          if (is_array($style)) {
            foreach ($style as $media => $media_style) {
              // check if its a internal or external css file
              if (filter_var($media_style, FILTER_VALIDATE_URL) === false && substr($media_style, 0, 2) != '//') {
                $theme_config['css'][][$media] = $theme_config['path'] . '/' . $media_style;
              }
              else {
                $theme_config['css'][][$media] = $media_style;
              }
            }
          }
          else {
            // check if its a internal or external css file
            if (filter_var($style, FILTER_VALIDATE_URL) === false && substr($style, 0, 2) != '//') {
              $theme_config['css'][] = $theme_config['path'] . '/' . $style;
            }
            else {
              $theme_config['css'][] = $style;
            }
          }
        }
      }
    }
  }

  /**
   * Adds inherited js and css to subthemes from parents.
   */
  private static function parseSubthemes() {
    foreach (self::$registry as &$theme_config) {
      // Skip themes which have no parent theme
      if ($theme_config['parent'] == false) {
        continue;
      }

      // Fetch parent config
      $parent_config = self::config($theme_config['parent']);

      // Merge js, css and templates
      $theme_config['js'] = array_merge(
        $parent_config['js'],
        $theme_config['js']
      );
      $theme_config['css'] = array_merge(
        $parent_config['css'],
        $theme_config['css']
      );
      $theme_config['template_paths'] += $parent_config['template_paths'];
    }
  }

}
