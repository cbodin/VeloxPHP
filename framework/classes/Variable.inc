<?php
/**
 * Velox variable class.
 *
 * Persistent variable storage. Serialize the variable before
 * inserting to the database to allow all serializable classes to be
 * stored.
 *
 * @package Velox
 * @since 1.0
 */
class Variable {

  /**
   * Caches all variables to prevent database load.
   *
   * @var array
   */
  private static $variables = array();

  /**
   * Flag to indicate if the database table is installed.
   *
   * @var bool
   */
  private static $tableInstalled = false;

  /**
   * Sets a variable.
   *
   * The variable will be stored serialized into the database.
   *
   * @param string $name
   *   A unique name to be able to retrieve the variable later.
   * @param mixed $value
   *   Any PHP variable that can be serialized.
   *
   * @return void
   */
  static function set($name, $value) {
    self::checkDb();

    $dbh = Database::get();

    // Check if we already have this variable stored in the database
    $sth = $dbh->prepare('
      SELECT
        COUNT(*)
      FROM
        velox_variables
      WHERE
        name = :name
    ');
    $sth->execute(array(
      ':name' => $name,
    ));
    $match = $sth->fetchColumn();

    // Update the row
    if ($match == 1) {
      $query = '
        UPDATE
          velox_variables
        SET
          value = :value
        WHERE
          name = :name
      ';
    }
    // Insert a new row to the database
    else {
      $query = '
        INSERT INTO
          velox_variables
          (value, name)
        VALUES
          (:value, :name)
      ';
    }

    $placeholders = array(
      ':name'  => $name,
      ':value' => serialize($value),
    );

    $sth = $dbh->prepare($query);
    $sth->execute($placeholders);

    self::$variables[$name] = array(
      'exists' => true,
      'data'   => $value,
    );
  }

  /**
   * Fetches a previously set variable.
   *
   * @param string $name
   * @param mixed $default optional
   *   The value to return if the variable is not set.
   *   Default: null
   *
   * @return mixed
   */
  static function get($name, $default = null) {
    self::checkDb();

    // Check if we got this variable cached.
    if (isset(self::$variables[$name])) {
      if (self::$variables[$name]['exists'] == false) {
        return $default;
      }

      return self::$variables[$name]['data'];
    }

    // Check if this variable is set
    $sth = Database::get()->prepare('
      SELECT
        value
      FROM
        velox_variables
      WHERE
        name = :name
    ');
    $sth->execute(array(
      ':name' => $name,
    ));
    $value = $sth->fetchColumn();

    if ($value) {
      self::$variables[$name] = array(
        'exists' => true,
        'data'   => unserialize($value),
      );

      return self::$variables[$name]['data'];
    }

    self::$variables[$name] = array(
      'exists' => false,
    );

    return $default;
  }

  /**
   * Deletes a variable from the database.
   *
   * @param string $name
   *   The name of the variable to delete.
   */
  static function del($name) {
    self::checkDb();

    // If the variable isn't cached do a delete query as it's
    // unnecessary to do a select query to check if it exists as it's
    // a big change that the variable is set if this function is
    // called.
    if (
      !isset(self::$variables[$name]) ||
      self::$variables[$name]['exists']
    ) {
      $sth = Database::get()->prepare('
        DELETE FROM
          velox_variables
        WHERE
          name = :name
      ');
      $sth->execute(array(
        ':name' => $name,
      ));
    }

    self::$variables[$name] = array(
      'exists' => false,
    );
  }

  /**
   * Checks if the table is installed. If not, creates a new table.
   */
  private static function checkDb() {
    // check if this function was called before
    if (!self::$tableInstalled) {
      // Get the active database key
      $active_db = Database::getActive();
      $driver = Database::getDriver();
      $dbh = Database::get();

      switch ($driver) {
        case 'sqlite':
          $sth = $dbh->prepare('
            SELECT
              name
            FROM
              sqlite_master
            WHERE
              type = :type AND
              name = :table
          ');
          $placeholders = array(
            ':type'  => 'table',
            ':table' => 'velox_variables',
          );
          break;

        case 'mysql':
          $sth = $dbh->prepare('
            SELECT
              table_name
            FROM
              information_schema.tables
            WHERE
              table_schema = :database AND
              table_name = :table
          ');
          $placeholders = array(
            ':database' => Database::getName(),
            ':table'    => 'velox_variables',
          );
          break;
      }

      $sth->execute($placeholders);
      $table = $sth->fetchColumn();

      if (!$table) {
        switch ($driver) {
          case 'mysql':
            $query = '
              CREATE TABLE velox_variables (
                `name` varchar(255) NOT NULL,
                `value` TEXT NOT NULL,
              PRIMARY KEY (`name`))
            ';
            break;

          case 'sqlite':
            $query = '
              CREATE TABLE `velox_variables` (
                `name` TEXT PRIMARY KEY,
                `value` TEXT NOT NULL
              )
            ';
            break;
        }

        $dbh->exec($query);
      }

      self::$tableInstalled = true;
    }
  }

}
