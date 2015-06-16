<?php
/**
 * Application settings file.
 *
 * Rename this file to settings.php and remember to change the
 * permissions to read-only once all settings are configured properly.
 * 0444 on a *nix system.
 */

// Used in the title
$settings['sitename'] = 'My Custom App';

// Override unsecure or secure site url if there are any problems with
// the autodetection. Do not include trailing slash.
// $settings['site_url']['unsecure'] = 'http://localhost/veloxphp';
// $settings['site_url']['secure'] = 'https://localhost/veloxphp';

// Database configuration
$settings['database'] = array(
  'default' => array(
    'driver'  => 'mysql',
    'host'    => 'localhost',
    'user'    => 'myusername',
    'pass'    => 'mypassword',
    'name'    => 'databasename',
    'port'    => '3306', // Optional
  ),
);

// Custom application settings
$settings['application'] = array();

// When development is set to false, no error messages or errors
// from other modules will show up. Remember to set this to false
// when running on the production server.
$settings['development'] = true;

// Enable error_logging to application/error.log file
$settings['error_logging'] = true;

// Enabled modules (Sass and VeloxFacebook are core modules and GA and Admin are custom modules)
$settings['modules'] = array(
  'Sass',
  'VeloxFacebook',
  'GoogleAnalytics',
  'Admin',
);

// Setup facebook settings
$settings['VeloxFacebook'] = array(
  'appId' => '318001688409384',
  'secret' => '2cdbedf8798ab4217163284ede5b7349', 
);

// Setup google analytics settings
$settings['GoogleAnalytics'] = array(
  'id' => 'UA-61136629-1',
  'page_track' => false,
);

// The default theme
$settings['theme'] = 'my-theme';

// A timezone supported by PHP. See
// http://php.net/manual/en/timezones.php
$settings['timezone'] = 'Europe/Stockholm';

// The lang attribute of the html tag.
$settings['language'] = 'en';

// Turn off clean urls.
// $settings['clean_urls'] = false;

// Specify a cron key which can be used to call the cron.php file via
// http. If not configured, the cron.php file can only be called using
// the php cli.
// $settings['cron_key'] = '';
