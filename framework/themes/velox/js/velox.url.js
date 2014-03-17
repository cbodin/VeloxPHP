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
   * Checks if we're on a secure connection.
   *
   * @type {Function}
   * @return {Boolean}
   */
  Velox.Url.https = function() {
    return 'https:' == document.location.protocol;
  };

})(jQuery);
