;(function($) {
  "use strict";

  /**
   * Object container for all Url methods.
   * 
   * @type {Object}
   */
  Velox.Url = {};

  /**
   * Generates a url to a given path.
   * 
   * @param  {String|null} path optional
   *   Omitting the path or setting it to null will generate a url to
   *   the frontpage.
   * @param  {Object} options optional
   *   An object with the following keys:
   *
   *   absolute: Set it to true to generate a absolute url.
   *             Default: false
   *
   *   fragment: Adds a fragment to the end of url (eg. '#frag'). Do
   *             not include the '#'.
   *             Default: none
   *
   *   scheme:   The scheme for this url (http or https)
   *             Default: https if we're on a secure connection, http
   *             otherwise.
   * 
   * @return {String}
   */
  Velox.Url.generate = function(path, options) {
    var settings = $.extend({}, {
      absolute: false,
      fragment: null,
      scheme:   this.https() ? 'https' : 'http'
    }, options);

    if (arguments.length == 0) {
      path = null;
    }

    var settingsKey = settings.scheme;

    if (settings.absolute) {
      settingsKey += 'FullPath';
    }
    else {
      settingsKey += 'BasePath';
    }

    var buildUrl = Velox.Settings.Framework[settingsKey];

    if (path !== null && path != '/') {
      buildUrl += path;
    }

    if (settings.fragment !== null) {
      buildUrl += '#' + settings.fragment;
    }

    return buildUrl;
  };

  /**
   * Generates a url to the currently active theme.
   *
   * @param {String} path
   *   The path to get a url for, relative to the theme directory.
   * @param {Object} options optional
   *   Passed directly to Velox.Url.generate().
   *
   * @return {String}
   *   The url to the theme or to the chosen path relative to the
   *   theme.
   */
  Velox.Url.generateTheme = function(path, options) {
    var buildPath = Velox.Settings.Framework.themePath;

    if (path !== null && path !== undefined) {
      buildPath += '/' + path;
    }

    return this.generate(buildPath, options);
  };

  /**
   * Returns a specific part of the current path. This will not be
   * updated when the href is changed using history api.
   *
   * @param {Integer} index optional
   *   A specific position to get the path at. If omitted or null the
   *   path will be returned either as a string or array depending on
   *   the asArray parameter.
   *   Default: none
   * @param {Boolean} asArray optional
   *   Returns the complete path as an array instead of a string.
   *
   * @return {String|Array}
   */
  Velox.Url.path = function(index, asArray) {
    if (!isNaN(index) && isFinite(index) && index !== null) {
      return Velox.Settings.Framework.urlPath[index];
    }

    if (asArray) {
      return Velox.Settings.Framework.urlPath;
    }

    return Velox.Settings.Framework.urlPath.join('/');
  };

  /**
   * Checks if we're on a secure connection.
   *
   * @type {Function}
   * @return {Boolean}
   */
  Velox.Url.https = function() {
    return 'https:' == document.location.protocol;
  };

})(jQuery);
