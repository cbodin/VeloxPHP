<?php
class SassActions {
  private static $file_ext = 'scss';

  static function css(&$css) {
    foreach ($css as &$stylesheet) {
      if ($stylesheet['type'] == 'internal') {
        $extension = pathinfo($stylesheet['data'], PATHINFO_EXTENSION);

        if (stripos($extension, self::$file_ext) === 0) {
          // Strip any query params from the path
          $extension_index = strripos($stylesheet['data'], self::$file_ext) + strlen(self::$file_ext);
          $params = substr($stylesheet['data'], $extension_index);
          $stylesheet['data'] = substr($stylesheet['data'], 0, $extension_index);
          
          // Compile stylesheet
          $stylesheet = Sass::compile($stylesheet);

          // Add back any query params
          $stylesheet['data'] .= $params;
        }
      }
    }
  }

}
