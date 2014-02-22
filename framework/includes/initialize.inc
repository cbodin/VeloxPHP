<?php
if (!defined('VELOX_ROOT')) {
  define('VELOX_ROOT', dirname(dirname(dirname(__FILE__))));
}

session_start();

// Cleanup magic quotes if they are turned on.
if (
  function_exists('get_magic_quotes_gpc') &&
  get_magic_quotes_gpc()
) {
  // Create lamba style unescaping function (for portability)
  $quotes_sybase = strtolower(ini_get('magic_quotes_sybase'));
  $unescape_function = 'str_replace("\'\'","\'",$value)';

  if (empty($quotes_sybase) || $quotes_sybase === 'off') {
    $unescape_function = 'stripslashes($value)';
  }

  $stripslashes_deep = create_function('&$value, $fn', '
    if (is_string($value)) {
      $value = ' . $unescape_function . ';
    } else if (is_array($value)) {
      foreach ($value as &$v) $fn($v, $fn);
    }
  ');

  // Unescape data
  $stripslashes_deep($_POST, $stripslashes_deep);
  $stripslashes_deep($_GET, $stripslashes_deep);
  $stripslashes_deep($_COOKIE, $stripslashes_deep);
  $stripslashes_deep($_REQUEST, $stripslashes_deep);
}

// Send P3P Compact Policy (CP) headers to prevent IE from blocking
// cookies.
header('P3P: CP="CAO PSA OUR"');

// Set a locale to make sure all date/time functions are working
// properly.
setlocale(LC_ALL, 'C');

// Make sure the internal encoding is UTF-8.
mb_internal_encoding('UTF-8');

$includes_path = VELOX_ROOT . '/framework/includes/';
$classes_path = VELOX_ROOT . '/framework/classes/';

// Includes
require_once $includes_path . 'constants.inc';

// Include classes.
require_once $classes_path . 'class.velox.inc';
require_once $classes_path . 'class.url.inc';
require_once $classes_path . 'class.settings.inc';
require_once $classes_path . 'class.module.inc';
require_once $classes_path . 'class.theme.inc';
require_once $classes_path . 'class.phptemplate.inc';
require_once $classes_path . 'class.router.inc';
require_once $classes_path . 'class.database.inc';
require_once $classes_path . 'class.resources.inc';

// TODO: Classes
// require_once(VELOX_ROOT . '/core/includes/class.session.inc');
// require_once(VELOX_ROOT . '/core/includes/class.message.inc');
// require_once(VELOX_ROOT . '/core/includes/class.variable.inc');

// Fake $_SERVER globals if the app is running via the cli as some
// classes depends on them.
if (VELOX_CLI) {
  $_SERVER['SERVER_NAME'] = 'localhost';
  $_SERVER['SERVER_PORT'] = 80;
  $_SERVER['SCRIPT_NAME'] = '/index.php';
  $_SERVER['REQUEST_URI'] = '/';
}

// Set timezone, default to UTC
$timezone = Settings::get('timezone', 'UTC');
date_default_timezone_set($timezone);

// Turn on all error reporting on development
if (Settings::get('development', false)) {
  ini_set('display_errors', 1);
  error_reporting(-1);
} else {
  ini_set('display_errors', 0);
  error_reporting(0);
}

// Include composer autoloader if it exists
if (file_exists(VELOX_ROOT . '/vendor/autoload.php')) {
  require_once VELOX_ROOT . '/vendor/autoload.php';
}
