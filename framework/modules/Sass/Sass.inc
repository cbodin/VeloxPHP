<?php
// Set a message if we are missing the Facebook library.
if (!class_exists('scssc')) {
  Message::set(
    'https://github.com/leafo/scssphp/ library missing. Install it using Composer.',
    Message::ERROR
  );
}
else {
  Module::loadInclude('Sass', 'classes/Sass.inc');
  Module::loadInclude('Sass', 'classes/SassActions.inc');

  // Set a very light weight to make sure ResourceCache is being run
  // after.
  Module::addActionCallback('css', 'SassActions::css', -50);
}
