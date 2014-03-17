<?php
class VeloxFacebookActions {

  /**
   * Listen for the render action and add the fb-root if its a normal
   * page output.
   */
  static function render(&$content, &$page_info) {
    if ($page_info['output callback'] == Response::NORMAL) {
      $settings = VeloxFacebook::getSettings();

      Theme::setPage('top', 'fb_root', '<div id="fb-root"></div>');

      Resources::addJs(array(
        'VeloxFacebook' => array(
          'appId' => $settings['appId'],
          'locale' => $settings['locale'],
          'channelUrl' => Url::generate('modules/veloxfacebook/channel.html', array(
            'absolute' => true,
          )),
        ),
      ), 'settings');
      
      Resources::addJs(array(
        'data' => Module::path('VeloxFacebook') . '/velox.facebook.js',
      ));
    }
  }

  /**
   * Add the channel page as a route.
   */
  static function routes() {
    $routes['modules/veloxfacebook/channel.html'] = array(
      'callback' => 'VeloxFacebook::channelPage',
      'output callback' => Response::BLANK,
    );

    return $routes;
  }

}
