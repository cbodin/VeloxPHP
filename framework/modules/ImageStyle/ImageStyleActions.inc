<?php
class ImageStyleActions {

  static function routeMatch($route_info) {
    // If there was no match, check if this is a imagecache request.
    if ($route_info === false) {
      ImageStyle::checkImageStyleMatch();
    }
  }
  
}
