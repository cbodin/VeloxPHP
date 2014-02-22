<?php
 /**
  * Velox database class.
  *
  * Sets up PDO objects connected to specified databases.
  * 
  * @package Velox
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
	 * @param string $key The database key to set as active
	 *
	 * @return void
	 */
	static function setActive($key) {
		self::$activeDatabase = $key;
	}

	/**
	 * Gets the active database key.
	 *
	 * @return string The currently active key
	 */
	static function getActive() {
		return self::$activeDatabase;
	}

	/**
	 * Adds a new database connection.
	 *
	 * Example:
	 *
	 * <code>
	 * Database::add('db2', array(
	 *  	'type' => 'mysql',
	 *  	'host' => 'localhost',
	 *  	'name' => 'dbname',
	 *  	'user' => 'dbuser',
	 *  	'pass' => 'dbpass',
	 * ));
	 * </code>
	 *
	 * <code>
	 * Database::add('db3', array(
	 *  	'type' => 'sqlite',
	 *  	'file' => VELOX_ROOT . '/application/db.sqlite',
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
	 * Checks if we already have a connection for the specificied server and returns that object.
	 * It will automatically connect if there is no active connection.
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
	 * $sth = Database::get('db2')->prepare('SELECT name FROM page WHERE id = :id');
	 * $result = $sth->execute(array(':id' => 5));
	 * </code>
	 *
	 * @param string $key
	 *   The key which identifies this connection.
	 *
	 * @return PDO
	 *   A pdo object connected to the speficied database.
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
	 * * Configure pdo settings to use UTF-8 and turns on exception error mode on development.
	 *
	 * @param string $key The database key defined in the app config.
	 *
	 * @return void
	 */
	private static function connect($key) {
		$db_config = Settings::get('database', array());

		if (!isset($db_config[$key])) {
			throw new Exception('Missing connection info for: ' . $key);
		}

		$server_info = $db_config[$key];

		// Validate database driver
		if (!in_array($server_info['driver'], array('mysql', 'sqlite'))) {
			throw new Exception('No support for database driver: ' . $server_info['driver']);
		}

		try {
			if ($server_info['driver'] == 'mysql') {
				$db = new PDO('mysql:host=' . $server_info['host'] . ';dbname=' . $server_info['name'] . ';charset=UTF8', $server_info['user'], $server_info['pass'], array(PDO::MYSQL_ATTR_INIT_COMMAND => 'SET NAMES utf8'));
			}
			elseif ($server_info['driver'] == 'sqlite') {
				$db = new PDO('sqlite:' . $server_info['file']);
			}
		} catch (PDOException $e) {
			throw new Exception('Could not connect to database ' . $key . '. PDO error: ' . $e->getMessage());
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
