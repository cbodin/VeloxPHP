<?php
/**
 * Response class.
 *
 * Sets correct header and encodes the content correctly depending
 * on output method.
 *
 * @package Velox
 * @since 1.0
 */
class Response {

  /**
   * Shorthand for Response::outputNormal();
   */
  const NORMAL = 'Response::outputNormal';

  /**
   * Shorthand for Response::outputJson();
   */
  const JSON = 'Response::outputJson';

  /**
   * Shorthand for Response::outputText();
   */
  const TEXT = 'Response::outputText';

  /**
   * Shorthand for Response::outputBlank();
   */
  const BLANK = 'Response::outputBlank';

  /**
   * Prints html and page template files wrapped around the content.
   */
  static function outputNormal($content) {
    // Set the correct content type
    header('Content-Type: text/html; charset=UTF-8');

    // Add the theme resources
    self::addThemeResources();

    // Add local jquery path, http/https BasePath, fullPath and
    // themePath to the js settings.
    Resources::addJs(array(
      'Framework' => array(
        'jqueryPath' => Url::generateTheme('js/jquery.min.js', array(
          'theme' => 'velox',
        )),
        'httpBasePath' => Url::generate(null, array(
          'scheme' => 'http',
        )),
        'httpsBasePath' => Url::generate(null, array(
          'scheme' => 'https',
        )),
        'httpFullPath' => Url::generate(null, array(
          'scheme' => 'http',
          'absolute' => true,
        )),
        'httpsFullPath' => Url::generate(null, array(
          'scheme' => 'https',
          'absolute' => true,
        )),
        'urlPath' => Url::path(null, true),
        'themePath' => Theme::path(),
      ),
    ), 'settings');

    /**
     * Javascript
     */
    $js = Resources::getJs();

    // Create the Velox js object.
    $js_settings = 'var Velox = Velox || {};';
    $js_settings .= 'Velox.App = {};';
    $js_settings .= 'Velox.Settings = ';
    $js_settings .= json_encode($js['settings']) . ';';

    // Add the Velox js object to the scripts.
    $velox_js = array(
      array(
        'id' => 'velox',
        'type' => 'inline',
        'data' => $js_settings,
      ),
    );
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

    // Add content variable
    $page_variables['content'] = $content;

    // Let modules interact before the final output.
    Module::alterAction('pageVariables', $page_variables);

    // render the page template
    $page_content = Theme::render('page', $page_variables);


    /**
     * Html template
     */
    $html_variables = Theme::getVariables('html');

    // Add default variables
    $html_variables['language'] = Settings::get('language');
    $html_variables['content'] = $page_content;
    $html_variables['classes_array'] = Theme::getBodyClasses();
    $html_variables['js'] = $js_html;
    $html_variables['css'] = $css_html;
    $html_variables['page_top'] = Theme::getPage('top');
    $html_variables['page_bottom'] = Theme::getPage('bottom');

    // Add default html head elements
    $html_head = Resources::getHtmlHead();

    // Force ie to not use compatibility mode and use chrome frame
    // when installed. 
    $html_head[] = array(
      'name' => 'meta',
      'attributes' => array(
        'http-equiv' => 'X-UA-Compatible',
        'content' => 'IE=edge,chrome=1',
      ),
    );

    $html_head[] = array(
      'name' => 'title',
      'content' => Velox::getTitle(),
    );

    $html_variables['head'] = $html_head;

    // Let modules interact before the final output.
    Module::alterAction('htmlVariables', $html_variables);

    // Concatenate body classes
    $html_variables['classes'] = implode(
      ' ',
      $html_variables['classes_array']
    );

    // Concatenate page top and page bottom
    $html_variables['page_top'] = implode(
      $html_variables['page_top']
    );
    $html_variables['page_bottom'] = implode(
      $html_variables['page_bottom']
    );

    // Build html tags of the head
    $html_variables['head'] = array_map(
      array('Resources', 'buildHtmlTag'),
      $html_variables['head']
    );

    // Concatenate html head
    $html_variables['head'] = implode(
      "\r\n",
      $html_variables['head']
    );

    return Theme::render('html', $html_variables);
  }

  /**
   * Prints the returned data as json without html and page template.
   */
  static function outputJson($content) {
    // Set the correct content type
    header('Content-type: application/json');
    return json_encode($content);
  }

  /**
   * Prints the returned data as html without html and page template.
   */
  static function outputText($content) {
    // Set the correct content type
    header('Content-type: text/plain; charset=UTF-8');
    return $content;
  }

  /**
   * Prints the returned data as plain text without html and page
   * template.
   */
  static function outputBlank($content) {
    return $content;
  }

  /**
   * Parses the theme config and adds css and javascript.
   */
  private static function addThemeResources() {
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
