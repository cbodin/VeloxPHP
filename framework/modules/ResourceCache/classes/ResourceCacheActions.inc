<?php
class ResourceCacheActions {

  static function js(&$js) {
    if (ResourceCache::cacheEnabled('js')) {
      $js_groups = ResourceCache::getResourceGroups($js['script']);
      $data = ResourceCache::concatenate($js_groups, 'script');

      $js['script'] = $data;
    }
  }

  static function css(&$css) {
    if (ResourceCache::cacheEnabled('css')) {
      $css_groups = ResourceCache::getResourceGroups($css);
      $css = ResourceCache::concatenate($css_groups, 'css');
    }
  }
  
}
