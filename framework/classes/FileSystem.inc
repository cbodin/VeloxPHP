<?php
/**
 * Velox FileSystem class.
 *
 * Provides a way to create directories, change file permissions,
 * copy files etc.
 *
 * @package Velox
 * @since 1.0
 */
class FileSystem {

  /**
   * Create a directory at the specified path.
   *
   * @param string $path
   *   The path to the directory.
   * @param int $mode optional
   *   The mode to set on the newly created directory.
   *   Default: 777
   * @param bool $recursive optional
   *   Recursively create missing directories up until the specified
   *   path.
   *   Default: true
   *
   * @return bool
   *   true if the directory could be created, false otherwise
   */
  static function mkdir($path, $mode = 0777, $recursive = true) {
    // Check if this is already a folder, this allows for always
    // calling this method just for making sure a folder always
    // exists.
    if (is_dir($path)) {
      return true;
    }

    $umask = umask(0);
    $created = mkdir($path, $mode, $recursive);
    umask($umask);

    return $created;
  }

  /**
   * Changes the mode on a file/directory.
   *
   * @param string $path
   *   The path to the file/directory.
   * @param int $mode
   *   The mode to set.
   *
   * @return bool
   *   true if the mode could be changed, false otherwise
   */
  static function chmod($path, $mode) {
    $umask = umask(0);
    $changed = chmod($path, $mode);
    umask($umask);

    return $changed;
  }

  /**
   * Returns the extension of the file.
   *
   * @param string $filepath
   *   The path to the file
   *
   * @return string
   *   The extension excluding the "."
   */
  static function extension($filepath) {
    return pathinfo($filepath, PATHINFO_EXTENSION);
  }

  /**
   * Returns the mime type of the file.
   *
   * @param string $filepath
   *   The path to the file
   *
   * @return string|bool
   *   The mime type of the file. E.g. image/png. If mimetype could
   *   not be determined false is returned.
   */
  static function mime($filepath) {
    if (!function_exists('finfo_open')) {
      return mime_content_type($filepath);
    }

    $finfo = finfo_open(FILEINFO_MIME_TYPE);
    $mime = finfo_file($finfo, $filepath);
    finfo_close($finfo);

    return $mime;
  }

  /**
   * Returns a unix timestamp when a file/directory was last modified.
   *
   * @param string $path
   *   The path to the file/directory. No trailing slash for
   *   directories.
   *
   * @return int|bool
   *   Unix timestamp or false if failure.
   */
  static function modified($path) {
    return filemtime($path);
  }

  /**
   * Copies a file from one directory to another.
   *
   * If the destination directory does not exist, the method will
   * return false.
   *
   * @param string $source
   *   The source file.
   * @param string $destination
   *   The folder to copy the file to. Do not include the filename.
   * @param bool $overwrite optional
   *   true to overwrite an existing file
   *   Default: false
   *
   * @return bool
   *   true if the file could be copied, false otherwise.
   */
  static function copy($source, $destination, $overwrite = false) {
    if (!is_dir($destination)) {
      return false;
    }

    // Make sure we got no trailing slashes
    $destination = rtrim($destination, '/');

    // Add the filename
    $destination .= '/' . basename($source);

    return copy($source, $destination);
  }

  /**
   * Moves a file/directory.
   *
   * @param string $source
   *   The path to the file/directory.
   * @param string $destination
   *   The folder to move the file/directory to. Do not include the
   *   filename/folder name of the source.
   * @param bool $overwrite optional
   *   true to overwrite an existing file
   *   Default: false
   *
   * @return bool
   *   true if the file could be moved, false otherwise.
   */
  static function move($source, $destination, $overwrite = false) {
    // Make sure we got no trailing slashes
    $destination = rtrim($destination, '/');

    // Add the filename
    $destination .= '/' . basename($source);

    if ($overwrite && file_exists($destination)) {
      return false;
    }

    return rename($source, $destination);
  }

  /**
   * Renames a file/directory.
   *
   * @param string $path
   *   The path to the file/directory.
   * @param string $new_name
   *   The new name of the file/directory.
   * @param bool $overwrite optional
   *   true to overwrite an existing file
   *   Default: false
   *
   * @return bool
   *   true if the file could be renamed, false otherwise.
   */
  static function rename($path, $new_name, $overwrite = false) {
    $new_path = dirname($path) . '/' . $new_name;

    if ($overwrite && file_exists($new_path)) {
      return false;
    }

    return rename($path, $new_path);
  }

  /**
   * Creates a safe filename.
   * 
   * All characters except a-z, 0-9 and _ will be removed to prevent
   * issues with different character encodings. The filename is also
   * lowercas'ed to prevent issues when copying files from a
   * case-sensitive filesystem to a case-insensitive filesystem.
   *
   * Spaces and - will be replaced by _. If the filename only consists
   * of non-safe characters, a randomly generate filename using
   * uniqid() will be used.
   *
   * @param string $filename
   *
   * @return string 
   */
  static function safeFilename($filename) {
    $ext = self::extension($filename);

    // Remove any extensions we might have
    if (!empty($ext)) {
      $ext = '.' . $ext;
      $ext_pos = strrpos($filename, $ext);
      $filename = substr($filename, 0, $ext_pos);
    }

    if (!empty($filename)) {
      // Sanitize the filename
      $match = array("/\s+/", "/[^a-zA-Z0-9\-]/", "/-+/", "/^-+/", "/-+$/");
      $replace = array("_","_","_","","");
      $filename = preg_replace($match, $replace, $filename);
      $filename = trim(preg_replace('/_+/', '_', $filename), '_');

      // We might end up with an empty filename in strings like 'åäö'.
      if (empty($filename)) {
        $filename = uniqid();
      }
    }

    return strtolower($filename . $ext);
  }

}
