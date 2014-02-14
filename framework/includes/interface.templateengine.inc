<?php
/**
 * Interface which all template engines must implement in order to
 * work properly with the system structure.
 */
interface TemplateEngine {

  /**
   * Renders a template file and returns the output.
   *
   * @param string $theme
   *   The name of the theme.
   * @param array $template_paths
   *   Array containing paths to all directories templates may be
   *   located inside. As themes can have a parent theme we need to
   *   do a search in both directories.
   * @param string $template.
   *   The name of the template.
   * @param array $variables
   *   The variables that should be available to the template.
   *
   * @return string
   *   The rendered template output.
   */
  static function render($theme, $template_paths, $template, $variables = array());

}
