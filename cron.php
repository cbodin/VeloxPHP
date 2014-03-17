<?php
define('VELOX_ROOT', dirname(__FILE__));
require VELOX_ROOT . '/framework/includes/initialize.inc';

Velox::initialize();

// If the cron.php file is not being called using cli, we need to see
// if we got a valid cron key in the request. 
if (!VELOX_CLI) {
  $cron_key = Settings::get('cron_key');

  // Do not allow cron to be run via http if the cron key is not
  // configured.
  if ($cron_key === null) {
    exit;
  }

  if (strcasecmp($cron_key,  Request::get('cron_key')) !== 0) {
    exit;
  }
}

Module::invokeAction('cron');
