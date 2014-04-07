<?php
Module::loadInclude('RedBeanPHP', 'lib/RedBeanPHP4_0_0/rb.phar');

/**
 * Configure RedBeanPHP.
 *
 * Configures all databases to RedBeanPHP.
 */
foreach (Settings::get('database', array()) as $dbkey => $dbinfo) {
  $method = 'addDatabase';

  // Setup RedBeanPHP with the default db
  if ($dbkey == 'default') {
    $method = 'setup';
  }

  $args = Database::getConnectionArgs($dbinfo);
  call_user_func_array(array('R', $method), $args);
}

// dont change the database after deployment
if (!Settings::get('development', false)) {
  R::freeze(true);
}
