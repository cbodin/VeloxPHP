<?php
class ResourceCache {

  private static $groupTemplate = array(
    'concatenate' => true,
    'data' => array(),
  );

  private static $currentCssDir;

  static function cacheEnabled($type) {
    $cache = Settings::get('ResourceCache', array());
    $cache += array(
      'css' => false,
      'js' => false,
    );

    return $cache[$type];
  }

  static function getResourceGroups($resources) {
    // Holds all groups which will either be concatenated or not
    $groups = array();

    // Holds every group which will be concatenated
    $group = self::$groupTemplate;

    foreach ($resources as $resource) {
      if ($resource['type'] == 'internal') {
        $group['data'][] = $resource;
      }
      else {
        if (sizeof($group['data'])) {
          // No need to concatenate when there's only one resource.
          if (sizeof($group['data']) == 1) {
            $group['concatenate'] = false;
          }

          // Add to the groups and reset group array
          $groups[] = $group;
          $group = self::$groupTemplate;
        }

        // Add the non-internal resource to the groups
        $groups[] = array(
          'concatenate' => false,
          'data' => array($resource),
        );
      }
    }

    // Add any group we might have left if the foreach ended on a
    // internal resource.
    if (sizeof($group['data'])) {
      $groups[] = $group;
    }

    return $groups;
  }

  static function concatenate($resources, $type) {
    $concatenated = array();

    foreach ($resources as $resource) {
      if ($resource['concatenate']) {

        if ($type == 'script') {
          $files = array();
          foreach ($resource['data'] as $script) {
            $files[] = $script['data'];
          }

          $concatenated[] = array(
            'type' => 'internal',
            'data' => self::concatenateJs($files),
          );
        }
        elseif ($type == 'css') {
          $concatenated[] = array(
            'media' => 'all',
            'type' => 'internal',
            'data' => self::concatenateCss($resource['data']),
          );
        }
      }
      else {
        $concatenated = array_merge($concatenated, $resource['data']);
      }
    }

    return $concatenated;
  }

  static function concatenateJs($files) {
    $filehash = md5(implode($files));
    $filename = 'application/files/cache/js_' . $filehash . '.js';

    // Check if this is already cached. When development is turned on
    // always rebuild cache.
    $build = !file_exists(VELOX_ROOT . '/' . $filename);
    $build = $build || Settings::get('development', false);
    if ($build) {
      self::ensureCacheDirExists();

      $data = '';

      foreach ($files as $file) {
        $data .= "/**\n";
        $data .= " * File: " . $file . "\n";
        $data .= " */\n";
        $data .= file_get_contents(VELOX_ROOT . '/' . $file);
        $data .= "\n\n";
      }

      file_put_contents(VELOX_ROOT . '/' . $filename, $data);
      chmod(VELOX_ROOT . '/' . $filename, 0666);
    }

    return $filename;
  }

  static function concatenateCss($css) {
    $files = array();

    foreach ($css as $style) {
      $files[] = $style['data'];
    }

    $filehash = md5(implode($files));
    $filename = 'application/files/cache/css_' . $filehash . '.css';

    // Check if this is already cached. When development is turned on
    // always rebuild cache.
    $build = !file_exists(VELOX_ROOT . '/' . $filename);
    $build = $build || Settings::get('development', false);
    if ($build) {
      self::ensureCacheDirExists();

      $data = '';

      foreach ($css as $style) {
        $data .= "/**\n";
        $data .= " * File: " . $style['data'] . "\n";
        $data .= " */\n";

        if ($style['media'] !== 'all') {
          $data .= '@media ' . $style['media'] . " {\n";
        }

        $content = file_get_contents(VELOX_ROOT . '/' . $style['data']);

        $data .= self::rewriteCssUrls($style['data'], $content);

        $data .= $style['media'] !== 'all' ? "\n}\n\n" : "\n\n";
      }

      file_put_contents(VELOX_ROOT . '/' . $filename, $data);
      chmod(VELOX_ROOT . '/' . $filename, 0666);
    }

    return $filename;
  }

  private static function rewriteCssUrls($filepath, $contents) {
    $css_dir = dirname($filepath) . '/';
    self::$currentCssDir = Url::generate($css_dir);

    $regexp = '/url\(\s*[\'"]?(?![a-z]+:|\/+)([^\'")]+)[\'"]?\s*\)/i';
    $callable = array(
      'self',
      'buildCssPath',
    );
    $data = preg_replace_callback($regexp, $callable, $contents);

    return $data;
  }

  private static function buildCssPath($matches) {
    $path = self::$currentCssDir . $matches[1];

    $regexp = '`(^|/)(?!\.\./)([^/]+)/\.\./`';
    $last = '';
    while ($path != $last) {
      $last = $path;
      $path = preg_replace($regexp, '$1', $path);
    }

    return "url('" . $path . "')";
  }

  private static function ensureCacheDirExists() {
    $cache_dir = VELOX_ROOT . '/application/files/cache';

    if (!is_dir($cache_dir)) {
      mkdir($cache_dir);
      chmod($cache_dir, 0666);
    }
  }

}
