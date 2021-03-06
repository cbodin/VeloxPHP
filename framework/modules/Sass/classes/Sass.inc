<?php
class Sass {

  private static $currentCssDir;

  static function compile($css) {
    // We include the whole filename so its easy to see which files
    // are compiled.
    $filename = substr_replace($css['data'], 'css', -4);
    $filename = 'application/files/cache/' . $filename;

    // Check if this is already cached. When development is turned on
    // always rebuild cache.
    $build = !file_exists(VELOX_ROOT . '/' . $filename);
    $build = $build || Settings::get('development', false);
    if ($build) {
      self::ensureCacheDirExists(dirname($filename));
      $scssc = new scssc();
      $scssc->addImportPath(VELOX_ROOT . '/' . dirname($css['data']));

      $content = file_get_contents(VELOX_ROOT . '/' . $css['data']);
      $data = $scssc->compile($content);
      $data = self::rewriteCssUrls($css['data'], $data);

      file_put_contents(VELOX_ROOT . '/' . $filename, $data);
      FileSystem::chmod(VELOX_ROOT . '/' . $filename, 0777);
    }

    $css['data'] = $filename;
    return $css;
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

  private static function ensureCacheDirExists($directory) {
    $cache_dir = VELOX_ROOT . '/' . $directory;

    FileSystem::mkdir($cache_dir, 0777);
  }

}
