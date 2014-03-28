<?php
if (!class_exists('RedBean_Facade')) {
  Message::set(
    'Missing RedBeanPHP library. Install it using Composer.',
    Message::ERROR
  );
}
else {
  class R extends RedBean_Facade {}

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
}
