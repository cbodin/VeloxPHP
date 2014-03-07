<?php
 /**
  * Velox message class
  *
  * Sets messages that will persist between page loads until they are
  * printed to the page or cleared manually
  *
  * @package Velox
  * @since 1.0
  */
class Message {

  /**
   * Status severity.
   */
  const SEVERITY_INFO = 'info';

  /**
   * Warning severity.
   */
  const SEVERITY_WARNING = 'warning';

  /**
   * Error severity.
   */
  const SEVERITY_ERROR = 'error';

  /**
   * Sets a message to be printed in the page template later on.
   *
   * @param string $string
   *   The string to be printed on the page.
   * @param string $severity optional
   *   One of the severity constants.
   *   Default self::SEVERITY_INFO
   * @param string $key optional
   *   A specific key to allow for overriding previous messages.
   *   Default: null. Will be appended as a numeric index.
   *
   * @return int|string
   *   The key that was set.
   */
  static function set($string, $severity = 'info', $key = null) {
    $message = array(
      'message' => $string,
      'severity' => $severity,
    );

    // Store the message in the session so it persists if redirects
    // occurs, only clear it when printing it to the page.
    $session = Session::get('velox', array());

    if (!isset($session['messages'])) {
      $session['messages'] = array();
    }

    // Add to messages
    if ($key === null) {
      $session['messages'][] = $message;

      // Retrieve the newly inserted key
      end($session['messages']);
      $key = key($session['messages']);
    }
    else {
      $session['messages'][$key] = $message;
    }

    Session::set('velox', $session);

    // Return the key that our message was set as.
    return $key;
  }

  /**
   * Sets a message using print_r() or krumo() if available.
   *
   * @param mixed $value
   *   The mixed variable that should be debugged.
   * @param string $key
   *   See self::set().
   *
   * @return mixed
   *   They key that was set.
   */
  static function debug($value, $key = null) {
    if (class_exists('krumo')) {
      $string = krumo::dump($value, KRUMO_CAPTURE);
    }
    else {
      $string = Resources::buildHtmlTag(array(
        'name' => 'pre',
        'content' => print_r($value, true),
      ));
    }

    return self::set($string, self::SEVERITY_INFO, $key);
  }

  /**
   * Gets all messages.
   *
   * @return array
   *   All messages
   */
  static function get() {
    $session = Session::get('velox', array());

    if (!isset($session['messages'])) {
      $session['messages'] = array();
    }

    return $session['messages'];
  }

  /**
   * Returns a string with all messages and possibly clears the queue.
   *
   * @param bool $clear optional
   *   Flag to indicate if the message queue should be cleared.
   *
   * @return string
   *   All messages wrapped in li.message tags in an ul#messages.
   */
  static function getString($clear = true) {
    $session = Session::get('velox', array());

    // Always return a empty string if there are no messages
    $messages = '';

    // Build string with messages
    if (isset($session['messages'])) {
      // TODO: Allow for these to be changed by class methods
      $item_template = array(
        'name' => 'li',
        'attributes' => array(
          'class' => array('message'),
        ),
      );
      $wrapper_template = array(
        'name' => 'ul',
        'attributes' => array(
          'id' => 'messages',
        ),
      );

      foreach ($session['messages'] as $message) {
        $item = $item_template;
        $item['attributes']['class'][] = $message['severity'];
        $item['content'] = $message['message'];
        $messages .= Resources::buildHtmlTag($item);
      }

      $wrapper_template['content'] = $messages;
      $messages = Resources::buildHtmlTag($wrapper_template);
    }

    if ($clear) {
      self::clear();
    }

    return $messages;
  }

  /**
   * Clears all message in the queue.
   */
  static function clear() {
    $session = Session::get('velox', array());
    $session['messages'] = array();
    Session::set('velox', $session);
  }

}
