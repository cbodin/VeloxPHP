<?php
class ImageStyleEffects {
  
  static function resize($image, $type, $options) {
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
    $width = imagesx($image);
    $height = imagesy($image);

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
      return $image;
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

    imagecopyresampled(
      $new_image, $image,
      0, 0,
      0, 0,
      $dest_width, $dest_height,
      $width, $height
    );

    imagedestroy($image);

    return $new_image;
  }

}
