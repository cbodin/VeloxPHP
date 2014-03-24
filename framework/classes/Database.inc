<?php
 /**
  * Velox database class.
  *
  * Sets up PDO objects connected to specified databases.
  *
  * @package Velox
  * @since 1.0
  */
class Database {

  /**
   * Stores all connected database objects.
   *
   * @var array
   */
  private static $connections = array();

  /**
   * The currently active database key.
   *
   * @var string
   */
  private static $activeDatabase = 'default';

  /**
   * Sets the active database key.
   *
   * @param string $key
   *   The database key to set as active
   */
  static function setActive($key) {
    self::$activeDatabase = $key;
  }

  /**
   * Gets the active database key.
   *
   * @return string
   *   The currently active key
   */
  static function getActive() {
    return self::$activeDatabase;
  }

  /**
   * Gets the driver of the selected database. If none is supplied it
   * will default to the active.
   *
   * @param string $key optional
   *   The database key.
   *   Default: the active database.
   *
   * @throws Exception
   *   If the database config for the key doesn't exist.
   *
   * @return string
   *   The driver of the active database.
   */
  static function getDriver($key = null) {
    if ($key === null) {
      $key = self::getActive();
    }

    $db_config = Settings::get('database', array());

    if (!isset($db_config[$key])) {
      throw new Exception('Missing connection info for: ' . $key);
    }

    return $db_config[$key]['driver'];
  }

  /**
   * Gets the name of the selected database. If none is supplied it
   * will default to the active. This will be the file url of a sqlite
   * database.
   *
   * @param string $key optional
   *   The database key.
   *   Default: the active database.
   *
   * @throws Exception
   *   If the database config for the key doesn't exist.
   *
   * @return string
   *   The name of the database if mysql or the filename if sqlite.
   */
  static function getName($key = null) {
    if ($key === null) {
      $key = self::getActive();
    }

    $db_config = Settings::get('database', array());

    if (!isset($db_config[$key])) {
      throw new Exception('Missing connection info for: ' . $key);
    }

    $driver = $db_config[$key]['driver'];

    switch ($driver) {
      case 'mysql':
        $name = $db_config[$key]['name'];
        break;

      case 'sqlite':
        $name = $db_config[$key]['file'];
        break;

      default:
        $error = 'Unsupported database driver: ' . $driver;
        throw new Exception($error);
    }

    return $name;
  }

  /**
   * Generate PDO connection arguments.
   *
   * @param array $db_info
   *   The database settings array.
   *
   * @return array
   *   The PDO constructor arguments.
   */
  static function getConnectionArgs($db_info) {
    $pdo_args = array(
      $db_info['driver'] . ':',
    );

    // Handle different database servers.
    switch ($db_info['driver']) {
      case 'mysql':
        $pdo_args[0] .= 'host=' . $db_info['host'];
        $pdo_args[0] .= ';dbname=' . $db_info['name'];
        $pdo_args[0] .= ';charset=utf-8';

        $pdo_args[1] = $db_info['user'];
        $pdo_args[2] = $db_info['pass'];

        $pdo_args[3] = array(
          PDO::MYSQL_ATTR_INIT_COMMAND => 'SET NAMES utf8',
        );
        break;

      case 'sqlite':
        $pdo_args[0] .= $db_info['file'];
        break;

      default:
        throw new Exception(
          'No support for database driver: ' . $db_info['driver']
        );
    }

    return $pdo_args;
  }

  /**
   * Adds a new database connection.
   *
   * Example:
   *
   * <code>
   * Database::add('db2', array(
   *    'driver' => 'mysql',
   *    'host' => 'localhost',
   *    'name' => 'dbname',
   *    'user' => 'dbuser',
   *    'pass' => 'dbpass',
   * ));
   * </code>
   *
   * <code>
   * Database::add('db3', array(
   *    'driver' => 'sqlite',
   *    'file' => VELOX_ROOT . '/application/db.sqlite',
   * ));
   * </code>
   *
   * @param string $key
   *   The key which identifies this connection.
   * @param array $server_info
   *   Server details.
   */
  static function add($key, $server_info) {
    global $databases;

    $databases[$key] = $server_info;
  }

  /**
   * Get a database connection.
   *
   * Checks if we got an open connection or creates a new one. This
   * allows for no connections to be made if a page request doesn't
   * need to query the database.
   *
   * Examples:
   *
   * * Get the default database connection.
   *
   * <code>
   * $db = Database::get();
   * </code>
   *
   * * Get a additional database connection.
   *
   * <code>
   * $result = Database::get('db2')
   *             ->prepare('SELECT name FROM page WHERE id = :id')
   *             ->execute(array(':id' => 5));
   * </code>
   *
   * @param string $key
   *   The key which identifies this connection.
   *
   * @return PDO
   *   A pdo object connected to the specified database.
   */
  static function get($key = null) {
    if ($key === null) {
      $key = self::$activeDatabase;
    }

    if (!isset(self::$connections[$key])) {
      self::connect($key);
    }

    return self::$connections[$key];
  }

  /**
   * Connects to a database.
   *
   * * Checks if there is any database settings for the specified key.
   * * Configure pdo settings to use UTF-8 and turns on exception
   *   error mode on development.
   *
   * @param string $key
   *   The database key defined in the app config.
   *
   * @throws Exception
   *   When a database connection fails.
   */
  private static function connect($key) {
    $db_config = Settings::get('database', array());

    if (!isset($db_config[$key])) {
      throw new Exception('Missing connection info for: ' . $key);
    }

    $db_info = $db_config[$key];

    $pdo_args = self::getConnectionArgs($db_info);

    // Make sure we catch this pdo error as PDO will throw an
    // exception containing the username and password.
    try {
      $reflection = new ReflectionClass('PDO');
      $db = $reflection->newInstanceArgs($pdo_args);
    } catch (PDOException $e) {
      $error = $e->getMessage();

      throw new Exception(
        'Could not connect to database ' . $key . ': ' . $error
      );
    }

    $db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);

    if (Settings::get('development', false)) {
      $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    }
    else {
      $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
    }

    self::$connections[$key] = $db;
  }
}
