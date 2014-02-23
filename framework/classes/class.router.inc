<?php
/**
 * Velox router class.
 *
 * Handles URL routing.
 * 
 * @package Velox
 * @since 1.0
 */
class Router {

  /**
   * Prints html and page template files wrapped around the content.
   */
  const PAGE_OUTPUT_NORMAL = 0;

  /**
   * Prints the returned data as json encoded without html and page
   * template.
   */
  const PAGE_OUTPUT_JSON = 1;

  /**
   * Prints the returned data as plain text encoded without html and
   * page template.
   */
  const PAGE_OUTPUT_BLANK = 2;

  /**
   * Matches the current URL to a specific route.
   *
   * Calls all routes actions to get available routes.
   * Each route will get a calculated score when a match is found and
   * the route with the highest score will be the one which is used.
   *
   * Available wildcards:
   * * :num         Matches any number.
   * * :alpha       Matches any letters.
   * * :alphanum    Matches any letters or numbers.
   * * :any         Matches any character.
   *
   * A special case exists for the frontpage which is matched against
   * :front only. So the frontpage route should be ':front'.
   *
   * @return bool|array
   *   The matched route or false if no route was found.
   */
  static function match() {
    $path_string = Url::path();
    $path_array = explode('/', $path_string);

    // Get all available routes
    $routes = Module::invokeAction('routes');

    // Store all matches
    $matches = array();

    // If an exact match was made, we will not use the scores
    $exact_match = false;

    foreach ($routes as $route => $route_info) {
      // Add the route to the route_info array to keep it.
      $route_info['route'] = $route;

      // Check if this is an exact match
      if (strcasecmp($path_string, $route) == 0) {
        $exact_match = $route_info;
        break;
      }

      $score = self::calculateScore($path_array, $route);

      if ($score > 0) {
        $route_info['score'] = $score;
        $matches[] = $route_info;
      }
    }

    $match = false;

    // If an exact match was made, return that.
    if ($exact_match) {
      $match = $exact_match;
    }
    elseif (!empty($matches)) {
      usort($matches, array('self', 'sortByScore'));
      $match = array_shift($matches);
    }

    // Add defaults
    if ($match != false) {
      if (!isset($match['type'])) {
        $match['type'] = self::PAGE_OUTPUT_NORMAL;
      }

      if (!isset($match['callback arguments'])) {
        $match['callback arguments'] = array();
      }

      // Make sure this is an absolute path to the file.
      if (
        isset($match['file']) &&
        !preg_match('/^(?:\/|\\\\|\w:\\\\|\w:\/).*$/', $match['file'])
      ) {
        $match['file'] = VELOX_ROOT . '/' . $match['file'];
      }
    }

    return $match;
  }

  /**
   * Calculates the score for a specific route.
   *
   * This is how the score is calculated:
   * * The url is splitted into parts by a forward slash '/'.
   * * An exact match of an part (no wildcard) gives 1 point.
   * * A wildcard gives 0.5 points.
   * * The points for each index is also calculated as:
   *   points / index + 1.
   *
   * Sample routes for the url 'blog/entry/3':
   * * 'blog/entry/:num' will have ~1.67 points (1/0.5/~0.17).
   * * 'blog/:alpha/3' will have ~1.58 points (1/0.25/~0.33).
   * * 'blog/:alpha/:num' will have ~1.42 points (1/0.25/~0.17).
   *
   * @return int
   *   The score for the path based in a route.
   */
  private static function calculateScore($path, $route) {
    // Check for the special ':front' which is the frontpage
    if (Url::isFront() && $route == ':front') {
      return 1;
    }

    $route_parts = explode('/', $route);

    $route_parts_count = sizeof($route_parts);
    $path_count = sizeof($path);

    // If the route parts and the path parts arent the same, we do not
    // have a match.
    if ($route_parts_count != $path_count) {
      return 0;
    }

    $score = 0;

    foreach ($route_parts as $index => $route_part) {
      $valid = false;
      $wildcard = true;

      switch ($route_part) {
        case ':num':
          if (is_numeric($path[$index])) {
            $valid = true;
          }
          break;

        case ':alpha':
          if (preg_match('/^[\p{L}]+$/u', $path[$index])) {
            $valid = true;
          }
          break;

        case ':alphanum':
          if (preg_match('/^[\p{L}0-9]+$/u', $path[$index])) {
            $valid = true;
          }
          break;

        case ':any':
          $valid = true;
          break;

        default:
          if (strcasecmp($path[$index], $route_part) == 0) {
            $valid = true;
            $wildcard = false;
          }
          break;
      }

      // If an route part didn't match we'll return 0 directly
      if (!$valid) {
        return 0;
      }

      if ($wildcard) {
        $score += 0.5 / ($index + 1);
      }
      else {
        $score += 1 / ($index + 1);
      }
    }

    return $score;
  }

  /**
   * usort compare function for sorting by score.
   *
   * @see match().
   */
  private static function sortByScore($a, $b) {
    if ($a['score'] == $b['score']) {
      return 0;
    }

    return ($a['score'] > $b['score']) ? -1 : 1;
  }
}
