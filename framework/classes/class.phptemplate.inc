<?php
/**
 * A simple template engine which uses regular PHP files as templates.
 *
 * Another template engine can easily be created by either extending
 * this class and overwriting the necessary methods or creating a new
 * class wich must implement the render method.
 *
 * @package Velox
 */
class PHPTemplate {

  /**
   * The file extensions for template files.
   */
  const FILE_EXTENSION = '.tpl.php';

  /**
   * Caches template paths for faster access.
   *
   * @var array
   */
  private static $templatePaths = array();

  /**
   * Renders a template file and returns the output.
   *
   * @param string $theme
   *   See PHPTemplate::getTemplate().
   * @param array $template_paths
   *   See PHPTemplate::getTemplate().
   * @param string $template.
   *   See PHPTemplate::getTemplate().
   * @param array $variables
   *   The variables that should be available to the template.
   *
   * @return string
   *   The rendered template output.
   */
  static function render($theme, $template_paths, $template, $variables = array()) {
    $template_path = self::getTemplate($theme, $template_paths, $template);

    // Remove unnecessary variables.
    unset($theme, $template_paths, $template);

    // Extract the variables to make them available in the template
    // file
    extract($variables);

    ob_start();
    include($template_path);
    $content = ob_get_clean();

    return $content;
  }

  /**
   * Finds the correct template.
   *
   * @param string $theme
   *   The name of the theme.
   * @param array $template_paths
   *   Array containing paths to all directories templates may be
   *   located inside. As themes can have a parent theme we need to
   *   do a search in both directories.
   * @param string $template.
   *   The name of the template.
   *
   * @return string
   *   The path to the template.
   */
  private static function getTemplate($theme, $template_paths, $template) {
    // Check if we got the template path cached.
    if (isset(self::$templatePaths[$theme][$template])) {
      $template_path = self::$templatePaths[$theme][$template];
    }
    else {
      $template_path = false;
      $template_filename = $template . self::FILE_EXTENSION;

      // Loop through all paths and check if we got a match.
      foreach ($template_paths as $path) {
        $file_path = $path . '/' . $template_filename;

        if (is_file(VELOX_ROOT . '/' . $file_path)) {
          $template_path = $file_path;
          break;
        }
      }
    }

    // Cache the path to the template.
    self::$templatePaths[$theme][$template] = $template_path;

    if ($template_path === false) {
      throw new Exception('Cannot find template: ' . $template);
    }

    return $template_path;
  }

}
