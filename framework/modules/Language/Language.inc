<?php
// Load the po parser class
Module::loadInclude('language', 'poparser.php');

/**
 * Language class.
 *
 * Allows for translations by using .po files.
 * 
 * @package Language
 */
class Language {

  /**
   * Stores all translation for all languages.
   *
   * @var array
   */
  private static $translations = array();

  /**
   * Holds the current locale that is being used for translations.
   *
   * @var null|string
   */
  private static $locale = null;

  /**
   * Sets the current locale that will be used when translate() is
   * being called.
   *
   * @param string $locale
   *   File prefix of the language file in the translations directory.
   */
  static function setLocale($locale) {
    self::$locale = $locale;
  }

  /**
   * Returns the current locale.
   *
   * @return null|string
   */
  static function getLocale() {
    if (self::$locale === null) {
      $settings = Settings::get('Language', array());

      if (!empty($settings['locale'])) {
        self::$locale = $settings['locale'];
      }
    }

    return self::$locale;
  }

  /**
   * Translate a message id to the currently set language.
   *
   * * Reads the translation file the first time translate is being
   * * called to avoid unnecessary file reads if translation is not
   * * needed on the current page request.
   *
   * @todo Add support for plural.
   *
   * @param string $msgid
   *   The msgid in the .po file
   * @param string $replacements
   *   Specify replacement tokens for translated string. Use @token
   *   for the text to be sanitized, !token for token that should be
   *   inserted as is.
   * @param string $locale
   *   Optionally specify another language than the default one.
   *
   * @return string
   *   The translated message if it exists in the current locale, if
   *   not, returns the msgid.
   */
  static function translate($msgid, $replacements = array(), $locale = null) {
    if ($locale === null) {
      $locale = self::getLocale();

      if ($locale === null) {
        return $msgid;
      }
    }

    if (!array_key_exists($locale, self::$translations)) {
      self::readTranslations($locale);
    }

    if (array_key_exists($msgid, self::$translations[$locale])) {
      $msgid = self::$translations[$locale][$msgid]['msgstr'][0];

      // Replace tokens inside msgid's
      foreach ($replacements as $replacement => $value) {
        $token = substr($replacement, 0, 1);

        // Sanitize everything
        if ($token == '@') {
          $value = filter_var($value, FILTER_SANITIZE_STRING);
        }

        // ! indicates it should be inserted as is, if thats not
        // specified remove it completely
        elseif ($token != '!') {
          $value = '';
        }

        $msgid = str_replace($replacement, $value, $msgid);
      }
    }

    return $msgid;
  }

  /**
   * Reads translations from .po files and stores them in the
   * $translations property.
   *
   * @param string $locale
   *   The locale of the .po file
   */
  private static function readTranslations($locale) {
    if (!self::validateLocale($locale)) {
      throw new Exception('Invalid locale: ' . $locale);
    }

    $file = VELOX_ROOT . '/application/translations/' . $locale . '.po';

    if (file_exists($file)) {
      $poparser = new PoParser();
      self::$translations[$locale] = $poparser->read($file);
    }
    else {
      self::$translations[$locale] = array();
      throw new Exception('Missing language file: ' . $file);
    }
  }

  /**
   * Validates the supplied locale.
   *
   * @param string $locale
   *
   * @return bool
   */
  private static function validateLocale($locale) {
    $locale = explode('_', $locale);

    // Make sure we got 2 indexes
    if (sizeof($locale) !== 2) {
      return false;
    }

    // Check that the size of both is 2
    if (strlen($locale[0]) !== 2 || strlen($locale[1]) !== 2) {
      return false;
    }

    if (!ctype_lower($locale[0]) || !ctype_upper($locale[1])) {
      return false;
    }

    return true;
  }

}
