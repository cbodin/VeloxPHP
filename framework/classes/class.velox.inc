<?php
/**
 * Velox class.
 *
 * Ties together the whole system.
 *
 * @package Velox
 * @since 1.0
 */
class Velox {

  /**
   * Initializes the framework.
   *
   * Adds the enabled modules and looks for all the themes.
   */
  static function initialize() {
    // Add enabled modules
    Module::addModules();

    // Call init actions
    foreach (Module::implementsAction('init') as $callable) {
      call_user_func($callable);
    }
  }

}
