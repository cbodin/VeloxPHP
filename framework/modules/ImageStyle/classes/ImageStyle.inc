<?php
/**
 * ImageStyle class.
 *
 * Allows for presets of image altering operations to be setup which
 * will save the resulting image inside the files directory.
 *
 * @package ImageStyle
 */
class ImageStyle {

  static function url($style, $image_path, $options = array()) {
    $path = self::path($style, $image_path);
    return Url::generate($path, $options);
  }

  static function themeUrl($style, $image_path, $options = array()) {
    $path = self::path($style, $image_path);
    return Url::generateTheme($path, $options);
  }

  static function path($style, $image_path) {
    return 'application/files/image-style/' . $style . '/' . $image_path;
  }

  static function createImage($style, $image) {
    $style_info = self::getStyleInfo($style);
    $quality = self::getQuality();

    // Make sure the style exists
    if (!$style_info) {
      throw new Exception('Missing image style: ' . $style);
    }

    $filename = basename($image);

    $image_dir = 'application/files/image-style/';
    $image_dir .= $style . '/' . dirname($image);

    // Create the directories if they dont exist already.
    if (!FileSystem::mkdir(VELOX_ROOT . '/' . $image_dir, 0777)) {
      throw new Exception('Could not create directories.');
    }

    // Validate image mime
    $image_type = exif_imagetype(VELOX_ROOT . '/' . $image);

    switch ($image_type) {
      case IMAGETYPE_JPEG:
        $type = 'jpeg';
        break;
      case IMAGETYPE_PNG:
        $type = 'png';
        break;
      default:
        throw new Exception('Unsupported image type.');
    }

    // Include effects class.
    Module::loadInclude('ImageStyle', 'classes/ImageStyleEffects.inc');

    $create_function = 'imagecreatefrom' . $type;
    $save_function = 'image' . $type;

    $source = VELOX_ROOT . '/' . $image;
    $image_gd = call_user_func($create_function, $source);

    if ($type == 'png') {
      imagealphablending($image_gd, true);
      imagesavealpha($image_gd, true);

      // Png output is in the range from 0 (best) to 9 (worst)
      $quality = round((100 - $quality) * 0.09);
    }

    foreach ($style_info as $effect) {
      $args = array(
        $image_gd,
        $type, 
      );

      if (isset($effect['options'])) {
        $args[] = $effect['options'];
      }

      Velox::ensureCallable($effect['effect']);
      $image_gd = call_user_func_array($effect['effect'], $args);
    }

    $destination = VELOX_ROOT . '/' . $image_dir . '/' . $filename;
    $result = call_user_func_array($save_function, array(
      $image_gd,
      $destination,
      $quality,
    ));

    if (!$result) {
      throw new Exception('Could not save image.');
    }

    FileSystem::chmod($destination, 0777);
    
    Url::reload();
  }

  static function checkImageStyleMatch($route_info) {
    // We will only check if this is a image style request if no route
    // is matched.
    if ($route_info !== false) {
      return;
    }

    $path = Url::path();
    $style_path = 'application/files/image-style/';

    // All image style images will be inside this path
    if (strpos($path, $style_path) === 0) {
      // Remove the path of the image-style folder
      $path = substr($path, strlen($style_path));

      $first_folder = strpos($path, '/');

      // Get the style
      $style = substr($path, 0, $first_folder);

      if (empty($style)) {
        return;
      }

      // Get the requested image
      $image = substr($path, strpos($path, '/', $first_folder) + 1);

      if (empty($image) || !self::validatePath($image)) {
        return;
      }

      if (!file_exists(VELOX_ROOT . '/' . $image)) {
        return;
      }

      self::createImage($style, $image);
    }
  }

  private static function getStyleInfo($style) {
    $settings = Settings::get('ImageStyle', array());

    if (!isset($settings['styles'][$style])) {
      return false;
    }

    return $settings['styles'][$style];
  }

  private static function getQuality() {
    $settings = Settings::get('ImageStyle', array());

    if (isset($settings['quality'])) {
      return $settings['quality'];
    }

    // Default to 75
    return 75;
  }

  private static function validatePath($path) {
    // We check for ".." to make sure directory traversal attacks are
    // not possible. Some further validation would probably be
    // necessary for maximum security.
    
    return strpos($path, '..') === false;
  }

}
