<?php
// Set a message if we are missing the Facebook library.
if (!class_exists('\Facebook\FacebookSession')) {
  Message::set(
    'Facebook library 4.x missing. Install it using Composer',
    Message::ERROR
  );
}
else {
  Module::loadInclude('VeloxFacebook4', 'classes/VeloxFacebook4.inc');
  Module::loadInclude('VeloxFacebook4', 'classes/VeloxFacebook4Actions.inc');

  Module::addActionCallback('render', 'VeloxFacebook4Actions::render');
  Module::addActionCallback('routes', 'VeloxFacebook4Actions::routes');

  // Setup VeloxFacebook4
  VeloxFacebook4::init();
}
