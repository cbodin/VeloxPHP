<?php
// The velox version number
define('VELOX_VERSION', '1.0');

// Define the current request time. Use this to improve performance
// over calling time() when you only need a non-precise time.
define('VELOX_TIME', (int) $_SERVER['REQUEST_TIME']);

// Is the app running using the php cli?
define('VELOX_CLI', php_sapi_name() === 'cli');

// True if static method callables are working as a string in the form
// "MyClass::myCallbackMethod". True for PHP >= 5.2.3
define('VELOX_STATIC_METHOD_STRING_CALLABLE', version_compare(PHP_VERSION, '5.2.3', '>='));
