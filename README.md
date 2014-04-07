VeloxPHP
========

A simple and fast PHP framework for rapid prototyping and small site building.

## Information
This is still in an early development phase. Features and API can change overnight.

## Installation
- Rename the *example.settings.php* file to *settings.php* inside the *application* folder.
- The *application/files* folder should be writable by the user running the script.
- (optionally) Create a composer.json file in the project root directory and add all external libraries you want. Velox will autoload them when they are being used. Read more about [Composer](http://getcomposer.org/).

## External libraries
Some built in modules requires some libraries to be installed via composer:

- The VeloxFacebook module requires the [Facebook PHP SDK](https://github.com/facebook/facebook-php-sdk).
- The Message::debug() method will output the code using [Krumo](https://github.com/oodle/krumo) if it's installed.
- The Sass module requires the [scssphp library](https://github.com/leafo/scssphp/).
