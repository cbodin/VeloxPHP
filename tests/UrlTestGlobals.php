<?php
class UrlTestGlobals extends PHPUnit_Framework_TestCase {
  protected static $settings;

  public static function setUpBeforeClass() {
      self::$settings = Settings::get('site_url');
      Settings::delete('site_url');
  }

  public static function tearDownAfterClass() {
      Settings::set('site_url', self::$settings);
  }

  public function testCanGenerateUnsecureUrl() {
    /**
     * Clean urls on.
     */
    Settings::set('clean_urls', true);

    $url = Url::generate();
    $this->assertEquals('/', $url);

    $url = Url::generate('veloxphp');
    $this->assertEquals('/veloxphp', $url);

    $url = Url::generate('veloxphp/');
    $this->assertEquals('/veloxphp/', $url);

    $url = Url::generate('veloxphp/front');
    $this->assertEquals('/veloxphp/front', $url);

    $url = Url::generate('veloxphp/front', array(
      'absolute' => true,
    ));
    $this->assertEquals('http://localhost/veloxphp/front', $url);

    /**
     * Clean urls off.
     */
    Settings::set('clean_urls', false);

    $url = Url::generate();
    $this->assertEquals('/', $url);

    $url = Url::generate('veloxphp');
    $this->assertEquals('/index.php/veloxphp', $url);

    $url = Url::generate('veloxphp/');
    $this->assertEquals('/index.php/veloxphp/', $url);

    $url = Url::generate('veloxphp/front');
    $this->assertEquals('/index.php/veloxphp/front', $url);

    $url = Url::generate('veloxphp/front', array(
      'absolute' => true,
    ));
    $this->assertEquals('http://localhost/index.php/veloxphp/front', $url);
  }

  public function testCanGenerateSecureUrl() {
    /**
     * Clean urls on.
     */
    Settings::set('clean_urls', true);

    $url = Url::generate(null, array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/', $url);

    $url = Url::generate('veloxphp', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxphp', $url);

    $url = Url::generate('veloxphp/', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxphp/', $url);

    $url = Url::generate('veloxphp/front', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxphp/front', $url);

    $url = Url::generate('veloxphp/front', array(
      'absolute' => true,
      'scheme' => 'https',
    ));
    $this->assertEquals('https://localhost/veloxphp/front', $url);

    /**
     * Clean urls off.
     */
    Settings::set('clean_urls', false);

    $url = Url::generate(null, array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/', $url);

    $url = Url::generate('veloxphp', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/index.php/veloxphp', $url);

    $url = Url::generate('veloxphp/', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/index.php/veloxphp/', $url);

    $url = Url::generate('veloxphp/front', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/index.php/veloxphp/front', $url);

    $url = Url::generate('veloxphp/front', array(
      'absolute' => true,
      'scheme' => 'https',
    ));
    $this->assertEquals('https://localhost/index.php/veloxphp/front', $url);
  }

  public function testCanGetCurrentUrl() {
    /**
     * Clean urls on.
     */
    Settings::set('clean_urls', true);

    $url = Url::current();
    $this->assertEquals('http://localhost/', $url);

    /**
     * Clean urls off.
     */
    Settings::set('clean_urls', false);

    $url = Url::current(array(
      'absolute' => false,
    ));
    $this->assertEquals('/', $url);
  }
}
