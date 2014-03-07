<?php
/**
 * Velox module class.
 *
 * Handles inclusion of modules and actions they can respond to.
 * 
 * @package Velox
 * @since 1.0
 */
class Module {

  /**
   * Enabled modules.
   * 
   * The application is really just a module and can be used the same
   * way as them. So we will enable it by default.
   *
   * @var array
   */
  private static $modules = array(
    'application' => 'application',
  );

  /**
   * Directories which should be scanned for modules.
   *
   * The order here is important as the first directory
   * self::findModules() finds a module inside will be the module of
   * choice. This allows for the application to override a framework
   * module if that should be necessary.
   *
   * @var array
   */
  private static $moduleDirectories = array(
    'application/modules',
    'framework/modules',
  );

  /**
   * Index of which modules implements a {Module Name}Actions class.
   *
   * @var array
   */
  private static $actionsIndex = array();

  /**
   * Returns an array of callable arrays with the class and method
   * which implements a specific action.
   *
   * @param string $action
   *   The action which we want all callables for. 
   *
   * @return array
   *   All callables which implements an action.
   */
  static function implementsAction($action) {
    $callables = array();

    foreach (self::$actionsIndex as $class) {
      if (method_exists($class, $action)) {
        $callables[] = array($class, $action);
      }
    }

    return $callables;
  }

  /**
   * Passes data to modules which implements the action and allows
   * them to alter it.
   *
   * @param string $action
   *   The action which should be called.
   * @param mixed $data
   *   The data which will be passed to the action.
   */
  static function alterAction($action, &$data) {
    foreach (self::implementsAction($action) as $callable) {
      call_user_func_array($callable, array(&$data));
    }
  }

  /**
   * Calls all the actions in all implementing modules and returns
   * the result.
   *
   * @param string $action
   *   The action which should be called.
   * @param mixed ... optional
   *   All additional arguments will be passed to the action callback.
   *
   * @return array
   *   The result of all actions merged recursively.
   */
  static function invokeAction($action) {
    $args = func_get_args();

    // Remove $action from the arguments.
    unset($args[0]);

    $return = array();
    foreach (self::implementsAction($action) as $callable) {
      $result = call_user_func_array($callable, $args);

      if (isset($result) && is_array($result)) {
        $return = array_merge_recursive($return, $result);
      }
      elseif (isset($result)) {
        $return[] = $result;
      }
    }

    return $return;
  }

  /**
   * Checks if a module is enabled.
   *
   * @param string $module
   *   The name of the module.
   *
   * @return bool
   */
  static function enabled($module) {
    return isset(self::$modules[$module]);
  }

  /**
   * Returns the path to a module.
   *
   * @param string $module
   *   The name of the module.
   * @param bool $absolute
   *   If set to true, it will return an absolute path from the server
   *   root.
   *
   * @return string|bool
   *   The path to the module if its enabled, or false if its not.
   */
  static function path($module, $absolute = false) {
    if (!self::enabled($module)) {
      return false;
    }

    if ($absolute) {
      return VELOX_ROOT . '/' . self::$modules[$module];
    }

    return self::$modules[$module];
  }

  /**
   * Includes a file relative from a modules directory.
   *
   * @param string $module
   *   The name of the module.
   * @param string $filename
   *   The name of the file to include.
   *
   * @return bool
   *   True if the file was found and included, false if not.
   */
  static function loadInclude($module, $filename) {
    $path = self::path($module, true);

    if ($path && file_exists($path . '/' . $filename)) {
      require_once $path . '/' . $filename;
      return true;
    }
    
    return false;
  }

  /**
   * Returns all enabled modules.
   *
   * @return array
   */
  static function getModules() {
    return self::$modules;
  }

  /**
   * Finds the correct path to the modules, adds them to the
   * self::$modules array and includes them in the
   * self::includeModule() method.
   */
  static function addModules() {
    $enabled_modules = Settings::get('modules', array());

    foreach ($enabled_modules as $module) {
      foreach (self::$moduleDirectories as $directory) {
        $module_path = $directory . '/' . $module;

        if (is_dir(VELOX_ROOT . '/' . $module_path)) {
          self::$modules[$module] = $module_path;

          // Stop after the first match in a directory
          continue 2;
        }
      }

      // If we got here the module wasn't found.
      throw new Exception('Cannot find module ' . $module);
    }

    self::loadModules();
  }

  /**
   * Loads all modules that was found in the self::addModules()
   * method by including the module file.
   */
  private static function loadModules() {
    foreach (self::$modules as $module => $module_path) {
      self::includeModule($module, $module_path);
    }
  }

  /**
   * Includes the module and checks if it implements an Actions class.
   *
   * @param string $module
   *   The name of the module.
   * @param string $path
   *   The path to the module directory.
   */
  private static function includeModule($module, $path) {
    require_once VELOX_ROOT . '/' . $path . '/' . $module . '.inc';

    if (class_exists($module . 'Actions')) {
      self::$actionsIndex[] = $module . 'Actions';
    }
  }

}
