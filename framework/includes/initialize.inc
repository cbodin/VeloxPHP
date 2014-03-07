<?php
if (!defined('VELOX_ROOT')) {
  define('VELOX_ROOT', dirname(dirname(dirname(__FILE__))));
}

session_start();

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
require_once $classes_path . 'velox.inc';
require_once $classes_path . 'url.inc';
require_once $classes_path . 'settings.inc';
require_once $classes_path . 'module.inc';
require_once $classes_path . 'theme.inc';
require_once $classes_path . 'phptemplate.inc';
require_once $classes_path . 'router.inc';
require_once $classes_path . 'database.inc';
require_once $classes_path . 'resources.inc';
require_once $classes_path . 'request.inc';
require_once $classes_path . 'session.inc';
require_once $classes_path . 'message.inc';
require_once $classes_path . 'variable.inc';

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

// Include composer autoload if it exists
if (file_exists(VELOX_ROOT . '/vendor/autoload.php')) {
  require_once VELOX_ROOT . '/vendor/autoload.php';
}
