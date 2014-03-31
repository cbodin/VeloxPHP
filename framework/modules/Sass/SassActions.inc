<?php
class SassActions {

  static function css(&$css) {
    foreach ($css as &$stylesheet) {
      if (
        $stylesheet['type'] == 'internal' &&
        pathinfo($stylesheet['data'], PATHINFO_EXTENSION) == 'scss'
      ) {
        $stylesheet = Sass::compile($stylesheet);
      }
    }
  }

}
