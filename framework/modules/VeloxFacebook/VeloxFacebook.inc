<?php
// Set a message if we are missing the Facebook library.
if (!class_exists('Facebook')) {
  Message::set(
    'Facebook library missing. Install it using Composer',
    Message::ERROR
  );
}
else {
  Module::loadInclude('VeloxFacebook', 'classes/VeloxFacebook.inc');
  Module::loadInclude('VeloxFacebook', 'classes/VeloxFacebookActions.inc');

  Module::addActionCallback('render', 'VeloxFacebookActions::render');
  Module::addActionCallback('routes', 'VeloxFacebookActions::routes');
}
