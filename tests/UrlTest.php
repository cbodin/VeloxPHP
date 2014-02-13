<?php
class UrlTest extends PHPUnit_Framework_TestCase {
  protected static $settings;

  public static function setUpBeforeClass() {
    self::$settings = Settings::get('site_url');
    Settings::set('site_url', array(
      'unsecure' => 'http://localhost/veloxphp',
      'secure' => 'https://localhost:1234/veloxsecurephp',
    ));
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
    $this->assertEquals('/veloxphp/', $url);

    $url = Url::generate('veloxphp');
    $this->assertEquals('/veloxphp/veloxphp', $url);

    $url = Url::generate('veloxphp/');
    $this->assertEquals('/veloxphp/veloxphp/', $url);

    $url = Url::generate('veloxphp/front');
    $this->assertEquals('/veloxphp/veloxphp/front', $url);

    $url = Url::generate('veloxphp/front', array(
      'absolute' => true,
    ));
    $this->assertEquals('http://localhost/veloxphp/veloxphp/front', $url);

    /**
     * Clean urls off.
     */
    Settings::set('clean_urls', false);

    $url = Url::generate();
    $this->assertEquals('/veloxphp/', $url);

    $url = Url::generate('veloxphp');
    $this->assertEquals('/veloxphp/index.php/veloxphp', $url);

    $url = Url::generate('veloxphp/');
    $this->assertEquals('/veloxphp/index.php/veloxphp/', $url);

    $url = Url::generate('veloxphp/front');
    $this->assertEquals('/veloxphp/index.php/veloxphp/front', $url);

    $url = Url::generate('veloxphp/front', array(
      'absolute' => true,
    ));
    $this->assertEquals('http://localhost/veloxphp/index.php/veloxphp/front', $url);
    
    Settings::set('clean_urls', true);
  }

  public function testCanGenerateSecureUrl() {
    /**
     * Clean urls on.
     */
    Settings::set('clean_urls', true);

    $url = Url::generate(null, array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxsecurephp/', $url);

    $url = Url::generate('veloxphp', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxsecurephp/veloxphp', $url);

    $url = Url::generate('veloxphp/', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxsecurephp/veloxphp/', $url);

    $url = Url::generate('veloxphp/front', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxsecurephp/veloxphp/front', $url);

    $url = Url::generate('veloxphp/front', array(
      'absolute' => true,
      'scheme' => 'https',
    ));
    $this->assertEquals('https://localhost:1234/veloxsecurephp/veloxphp/front', $url);

    /**
     * Clean urls off.
     */
    Settings::set('clean_urls', false);

    $url = Url::generate(null, array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxsecurephp/', $url);

    $url = Url::generate('veloxphp', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxsecurephp/index.php/veloxphp', $url);

    $url = Url::generate('veloxphp/', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxsecurephp/index.php/veloxphp/', $url);

    $url = Url::generate('veloxphp/front', array(
      'scheme' => 'https',
    ));
    $this->assertEquals('/veloxsecurephp/index.php/veloxphp/front', $url);

    $url = Url::generate('veloxphp/front', array(
      'absolute' => true,
      'scheme' => 'https',
    ));
    $this->assertEquals('https://localhost:1234/veloxsecurephp/index.php/veloxphp/front', $url);

    Settings::set('clean_urls', true);
  }

  public function testCanGetCurrentUrl() {
    /**
     * Clean urls on.
     */
    Settings::set('clean_urls', true);

    $url = Url::current();
    $this->assertEquals('http://localhost/veloxphp/', $url);

    /**
     * Clean urls off.
     */
    Settings::set('clean_urls', false);

    $url = Url::current(array(
      'absolute' => false,
    ));
    $this->assertEquals('/veloxphp/', $url);

    Settings::set('clean_urls', true);
  }
}
