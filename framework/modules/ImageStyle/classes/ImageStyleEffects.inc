<?php
class ImageStyleEffects {
  
  static function resize($image, $options = array()) {
    // Validate options.
    if (!isset($options['width']) && !isset($options['height'])) {
      throw new Exception('Missing width and height.');
    }

    // Add defaults.
    $options += array(
      'width' => 0,
      'height' => 0,
      'preserve-ratio' => true,
      'fit' => 'contain',
      'upscaling' => true,
    );

    // Get image width and height
    $width = imagesx($image->resource);
    $height = imagesy($image->resource);

    // If width or height is not defined we need to enforce preserve
    // ratio
    if ($options['width'] == 0 || $options['height'] == 0) {
      $options['preserve-ratio'] = true;
      $options['fit'] = 'contain';

      if ($options['width'] == 0) {
        $ratio = $width / $height;
        $options['width'] = ceil($options['height'] * $ratio);
      }
      else {
        $ratio = $height / $width;
        $options['height'] = ceil($options['width'] * $ratio);
      }
    }

    // Check if we upscaling is disabled. If it is we cannot resize
    // the image if its smaller than the requested width or height.
    if (
      !$options['upscaling'] &&
      ($options['width'] > $width || $options['height'] > $height)
    ) {
      return $image->resource;
    }

    if ($options['preserve-ratio']) {
      $ratio_w = $options['height'] / $height;
      $ratio_h = $options['width'] / $width;

      if ($options['fit'] == 'cover') {
        $ratio = max($ratio_w, $ratio_h);
      }
      else {
        $ratio = min($ratio_w, $ratio_h);
      }

      $dest_width = ceil($width * $ratio);
      $dest_height = ceil($height * $ratio);
    }
    else {
      $dest_width = $options['width'];
      $dest_height = $options['height'];
    }

    $new_image = imagecreatetruecolor(
      $dest_width,
      $dest_height
    );

    if ($image->type == 'png') {
      imagealphablending($new_image, true);
      imagesavealpha($new_image, true);

      // Fill the new image with transparent to make sure transparency
      // is preserved when resizing.
      imagefill($new_image, 0, 0, 0x7fff0000);
    }

    imagecopyresampled(
      $new_image, $image->resource,
      0, 0,
      0, 0,
      $dest_width, $dest_height,
      $width, $height
    );

    imagedestroy($image->resource);
    $image->resource = $new_image;
  }
  
  static function crop($image, $options) {
    // Validate options.
    if (!isset($options['width']) || !isset($options['height'])) {
      throw new Exception('Missing width or height.');
    }

    // Add defaults.
    // The anchor is specified as "X Y". Valid X values are: "left",
    // "center" and "right". Valid Y values are "top", "center" and
    // "bottom".
    $options += array(
      'anchor' => 'center center',
      'fill-background-jpeg' => '#ffffff',
      'fill-background-png' => 'transparent',
    );

    // Validate anchors
    $anchors = explode(' ', $options['anchor']);
    if (sizeof($anchors) != 2) {
      throw new Exception('Invalid anchor value');
    }

    $new_image = imagecreatetruecolor(
      $options['width'],
      $options['height']
    );

    if ($image->type == 'png') {
      imagealphablending($new_image, true);
      imagesavealpha($new_image, true);
    }

    switch ($image->type) {
      case 'jpeg':
        $rgb = self::hex2rgb($options['fill-background-jpeg']);
        $color = imagecolorallocate(
          $new_image,
          $rgb[0], $rgb[1], $rgb[2]
        );
        break;
      case 'png':
        if ($options['fill-background-png'] == 'transparent') {
          $color = 0x7fff0000;
        }
        else {
          $rgb = self::hex2rgb($options['fill-background-png']);
          $color = imagecolorallocate(
            $new_image,
            $rgb[0], $rgb[1], $rgb[2]
          );
        }
        break;
    }

    imagefill($new_image, 0, 0, $color);

    // Get image width and height
    $width = imagesx($image->resource);
    $height = imagesy($image->resource);

    switch ($anchors[0]) {
      case 'left':
        $left = 0;
        break;
      case 'right':
        $left = $options['width'] - $width;
        break;
      default:
        $left = ceil(($options['width'] - $width) / 2);  
    }

    switch ($anchors[1]) {
      case 'top':
        $top = 0;
        break;
      case 'bottom':
        $top = $options['height'] - $height;
        break;
      default:
        $top = ceil(($options['height'] - $height) / 2); 
    }

    imagecopy(
      $new_image, $image->resource,
      $left, $top, 
      0, 0, 
      $width, $height
    );

    imagedestroy($image->resource);
    $image->resource = $new_image;
  }

  static function exifRotate($image, $options = array()) {
    $exif = exif_read_data($image->file);

    $orientation = 0;

    if (isset($exif['Orientation'])) {
      switch ($exif['Orientation']) {
        case 3:
          $orientation = 180;
          break;

        case 6:
          $orientation = -90;
          break;

        case 8:
          $orientation = 90;
          break;
      }
    }

    if ($orientation !== 0) {
      $new_image = imagerotate($image->resource, $orientation, 0);
      imagedestroy($image->resource);
      $image->resource = $new_image;
    }
  }

  private static function hex2rgb($hex) {
    $hex = str_replace('#', '', $hex);

    if (strlen($hex) == 3) {
      $r = hexdec(substr($hex, 0, 1) . substr($hex, 0, 1));
      $g = hexdec(substr($hex, 1, 1) . substr($hex, 1, 1));
      $b = hexdec(substr($hex, 2, 1) . substr($hex, 2, 1));
    }
    else {
      $r = hexdec(substr($hex, 0, 2));
      $g = hexdec(substr($hex, 2, 2));
      $b = hexdec(substr($hex, 4, 2));
    }

    return array($r, $g, $b);
  }
}
