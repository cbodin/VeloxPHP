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
    // Turn on output buffering to capture all output so nothing gets
    // outputted before we've set all the headers.
    ob_start();

    // Add enabled modules
    Module::addModules();

    // Initialize the theme class
    Theme::initialize();

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

    // Default delivery type
    $delivery_type = Router::PAGE_OUTPUT_NORMAL;

    // Notify modules that we tried to find a matching route.
    Module::invokeAction('routeMatch', $page_info);

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

      $content = call_user_func_array(
        $page_info['callback'],
        $arguments
      );

      // check if we got any content back
      if ($content === false) {
        $content = self::error404();

        Theme::setActive($current_theme);
      }
      else {
        // Set page title
        if (array_key_exists('title', $page_info)) {
          self::setTitle($page_info['title']);
        }

        // check if we explicitly added a page callback type
        if (array_key_exists('type', $page_info)) {
          $delivery_type = $page_info['type'];
        }
      }
    }
    else {
      $content = self::error404();
    }

    if ($delivery_type == Router::PAGE_OUTPUT_NORMAL) {
      // Parse the theme config
      self::parseThemeConfig();

      // Add local jquery path to the js settings.
      Resources::addJs(array(
        'jqueryPath' => Url::generate('framework/js/jquery.min.js'),
      ), 'settings');

      /**
       * Javascript
       */
      $js = Resources::getJs();

      // Create the settings object
      $js_settings = 'var Velox = Velox || {};';
      $js_settings .= 'Velox.App = {};';
      $js_settings .= 'Velox.Settings = ';
      $js_settings .= json_encode($js['settings']) . ';';

      // Add all velox js at the top. Start with the settings as the
      // other velox js depends in it.
      $velox_js = array(
        array(
          'id' => 'velox-settings',
          'type' => 'inline',
          'data' => $js_settings,
        ),
        array(
          'id' => 'jquery',
          'type' => 'external',
          'data' => '//ajax.googleapis.com/ajax/libs/jquery/1.11.0/jquery.min.js',
        ),
        array(
          'id' => 'jquery-fallback',
          'type' => 'internal',
          'data' => 'framework/js/jquery.fallback.js',
        ),
      );

      // Add the framework js at the top.
      $js['script'] = array_merge($velox_js, $js['script']);

      // Let modules interact with the js before the final output.
      Module::alterAction('js', $js);

      $script_tags = array();

      foreach ($js['script'] as $script) {
        $script_tag = array(
          'name' => 'script',
        );

        if ($script['type'] == 'inline') {
          $script_tag['content'] = $script['data'];
        }
        else {
          // generate absolute url for internal files
          if ($script['type'] == 'internal') {
            $script['data'] = Url::generate($script['data']);
          }

          $script_tag['attributes']['src'] = $script['data'];
        }

        $script_tags[] = Resources::buildHtmlTag($script_tag);
      }

      $js_html = "\r\n" . implode("\r\n", $script_tags) . "\r\n";


      /**
       * Css
       */
      $css = Resources::getCss();

      // Let modules interact with the css before the final output.
      Module::alterAction('css', $css);

      $style_tags = array();

      foreach ($css as $style) {
        if ($style['type'] == 'inline') {
          $style_tag = array(
            'name' => 'style',
            'content' => $style['data'],
            'attributes' => array(
              'media' => $style['media'],
            ),
          );
        }
        else {
          if ($style['type'] == 'internal') {
            $style['data'] = Url::generate($style['data']);
          }

          $style_tag = array(
            'name' => 'link',
            'attributes' => array(
              'rel' => 'stylesheet',
              'media' => $style['media'],
              'href' => $style['data'],
            ),
          );
        }

        $style_tags[] = Resources::buildHtmlTag($style_tag);
      }

      $css_html = "\r\n" . implode("\r\n", $style_tags) . "\r\n";


      /**
       * Page template
       */
      $page_variables = Theme::getVariables('page');

      // Add default variables
      $page_variables['content'] = $content;
      $page_variables['page_title'] = self::$pageTitle;

      // Let modules interact before the final output.
      Module::alterAction('pageVariables', $page_variables);

      // render the page template
      $page_content = Theme::render('page', $page_variables);


      /**
       * Html template
       */
      $html_variables = Theme::getVariables('html');

      // Add default variables
      $html_variables['content'] = $page_content;
      $html_variables['classes'] = Theme::getBodyClasses();
      $html_variables['js'] = $js_html;
      $html_variables['css'] = $css_html;
      $html_variables['page_top'] = Theme::getPage('top');
      $html_variables['page_bottom'] = Theme::getPage('bottom');

      // Let modules interact before the final output.
      Module::alterAction('htmlVariables', $html_variables);

      // Concatenate body classes
      $html_variables['classes'] = implode(
        ' ',
        $html_variables['classes']
      );

      // Concatenate page top and page bottom
      $html_variables['page_top'] = implode(
        $html_variables['page_top']
      );
      $html_variables['page_bottom'] = implode(
        $html_variables['page_bottom']
      );

      $content = Theme::render('html', $html_variables);

      header('Content-Type: text/html; charset=UTF-8');
      ob_end_flush();
    }
    elseif ($delivery_type == Router::PAGE_OUTPUT_BLANK) {
      ob_end_flush();
    }
    elseif ($delivery_type == Router::PAGE_OUTPUT_JSON) {
      header('Content-type: application/json');
      ob_end_clean();
      $content = json_encode($content);
    }

    // Notify modules that we're about to display the page.
    Module::invokeAction('display', $content, $delivery_type);

    print $content;
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
    Theme::setVariable('html', 'page_title', $title);
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
   * Error 404 page.
   *
   * @return string
   *   The content of the 404 page.
   */
  private static function error404() {
    self::setTitle('Page not found');
    return 'Page not found: ' . Url::path();
  }

  /**
   * Parses the theme config and adds css and javascript.
   */
  private static function parseThemeConfig() {
    $theme_config = Theme::config();

    foreach ($theme_config['css'] as $stylesheet) {
      // if the stylesheet is an array, we have media queries
      if (is_array($stylesheet)) {
        foreach ($stylesheet as $media => $media_stylesheet) {
          $css_info = self::parseThemeCss($media, $media_stylesheet);
        }
      }
      // if its not, no media query was defined. Default to all.
      else {
        $css_info = self::parseThemeCss('all', $stylesheet);
      }

      Resources::addCss($css_info);
    }

    foreach ($theme_config['js'] as $script) {
      // check if its a internal or external js file
      if (
        filter_var($script, FILTER_VALIDATE_URL) === false &&
        substr($script, 0, 2) != '//'
      ) {
        $js_info = array(
          'type' => 'internal',
          'data' => $script,
        );
      }
      else {
        $js_info = array(
          'type' => 'external',
          'data' => $script,
        );
      }

      Resources::addJs($js_info);
    }
  }

  /**
   * Parses a theme css string and checks if its an internal or
   * external link.
   *
   * @param string $media
   *   The media attribute of the style.
   * @param string $stylesheet
   *   The stylesheet string from the theme config.
   *
   * @return array
   */
  private static function parseThemeCss($media, $stylesheet) {
    // check if its a internal or external css file
    if (
      filter_var($stylesheet, FILTER_VALIDATE_URL) === false &&
      substr($stylesheet, 0, 2) != '//'
    ) {
      $css_info = array(
        'media' => $media,
        'type' => 'internal',
        'data' => $stylesheet,
      );
    }
    else {
      $css_info = array(
        'media' => $media,
        'type' => 'external',
        'data' => $stylesheet,
      );
    }

    return $css_info;
  }

}
