<?php
/**
 * Velox resources class.
 *
 * Handles all css and javascript for the site.
 *
 * @package Velox
 * @since 1.0
 */
class Resources {

  /**
   * Stores all javascript.
   *
   * @var array
   */
  private static $js = array(
    'script' => array(),
    'settings' => array(),
  );

  /**
   * Stores all css.
   *
   * @var array
   */
  private static $css = array();

  /**
   * Contains all void elements in valid HTML as defined on
   * http://www.w3.org/TR/html-markup/syntax.html
   *
   * @var array
   */
  private static $voidElements = array(
    'area', 'base', 'br', 'col', 'command', 'embed', 'hr', 'img',
    'input', 'keygen', 'link', 'meta', 'param', 'source', 'track',
    'wbr',
  );

  /**
   * Add javascript files, inline javascript or javascript settings.
   *
   * Settings you add will be available under the global
   * Velox.Settings object.
   *
   * Examples:
   *
   * * Add an internal script
   *
   * <code>
   * $js = array(
   *  'type' => 'internal',
   *  'data' => 'app/js/phpjs.js',
   * );
   * Resources::addJs($js);
   * </code>
   *
   * * Add an external script
   *
   * <code>
   * $js = array(
   *  'type' => 'external',
   *  'data' => '//cdn.example.com/libs/script/script.min.js',
   * );
   * Resources::addJs($js);
   * </code>
   *
   * * Add inline script
   *
   * <code>
   * $js = array(
   *  'type' => 'inline',
   *  'data' => 'document.write("Hello World");',
   * );
   * Resources::addJs($js);
   * </code>
   *
   * * Add inline script with optional id key, which is good practice
   *   so other modules can remove or alter your output by knowing
   *   the id.
   *
   * <code>
   * $js = array(
   *  'id' => 'hello-world',
   *  'type' => 'inline',
   *  'data' => 'document.write("Hello World");',
   * );
   * Resources::addJs($js);
   * </code>
   *
   * * Add settings
   *
   * <code>
   * $js = array(
   *  'pageurls' => array(
   *    'page/info',
   *    'page/contact',
   *  ),
   * );
   * Resources::addJs($js, 'settings');
   * </code>
   *
   * @param array $info
   *   Array containing information about the script added.
   * @param string $type
   *   Type of js to add.
   */
  static function addJs($info, $type = 'script') {
    // Handle setting explicitly
    if ($type == 'settings') {
      $settings = self::$js['settings'];

      // Merge the new settings with the current ones
      self::$js['settings'] = array_merge_recursive($settings, $info);
    }
    else {
      // Add defaults
      $info += array(
        'type' => 'internal',
      );

      // add it to the js array
      self::$js['script'][] = $info;
    }
  }

  /**
   * Get all or specific js previously added.
   *
   * Returns all js or script/settings.
   *
   * @param string|null $type
   *   The type to retrieve. Either 'script' or 'settings'.
   *
   * @return array
   *   The specified type or all javascript.
   */
  static function getJs($type = null) {
    if ($type !== null) {
      return self::$js[$type];
    }
    
    return self::$js;
  }

  /**
   * Add css files/styles.
   *
   * Add internal/external or inline styles to be outputted in the
   * html template.
   *
   * * The media key in $info specifies the media query to use.
   *
   * Examples:
   *
   * * Add an internal style
   *
   * <code>
   * $style = array(
   *  'type' => 'internal',
   *  'media' => 'all',
   *  'data' => 'app/css/reset.css',
   * );
   * Resources::addCss($style);
   * </code>
   *
   * * Add an external style
   *
   * <code>
   * $style = array(
   *  'type' => 'external',
   *  'media' => 'screen',
   *  'data' => 'http://styles.cdn.com/style.css',
   * );
   * Resources::addCss($style);
   * </code>
   *
   * * Add inline style
   *
   * <code>
   * $style = array(
   *  'type' => 'inline',
   *  'data' => 'body { color: #fff; };',
   * );
   * Resources::addCss($style);
   * </code>
   *
   * @param array $info
   *   Array containing information about the style added.
   */
  static function addCss($info) {
    // Add defaults
    $info += array(
      'media' => 'all',
      'type' => 'internal',
    );

    // add it to the css array
    self::$css[] = $options;
  }

  /**
   * Get all styles previously added.
   *
   * Returns all added styles.
   *
   * @return array
   *   All css added up to the point of the call.
   */
  static function getCss() {
    return self::$css;
  }

  /**
   * Builds a html tag from an array.
   *
   * @param array $options
   *   An associative array containing:
   *     'name'
   *       The name of the tag.
   *     'attributes'
   *       Array with the key as the attribute and the value is the
   *       attribute content.
   *     'content'
   *       For non-void elements this is the content of the tag
   *
   * @return string
   *   The html tag
   */
  static function buildHtmlTag($options) {
    $tag = '<' . $options['name'];

    if (isset($options['attributes'])) {
      foreach ($options['attributes'] as $attribute => $value) {
        $tag .= ' ' . $attribute . '="' . $value .'"';
      }
    }

    $tag .= '>';

    if (!in_array($options['name'], self::$voidElements)) {
      if (!empty($options['content'])) {
        $tag .= $options['content'];
      }

      $tag .= '</' . $options['name'] . '>';
    }

    return $tag;
  }
}
