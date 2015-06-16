<?php

/**
* Add the classes for the Application here
*/
Module::loadInclude('application', 'classes/Application.inc');
Module::loadInclude('application', 'classes/ApplicationActions.inc');

/**
* Add the Action callbacks for the application
*/
Module::addActionCallback('routes', 'ApplicationActions::routes');
Module::addActionCallback('init', 'ApplicationActions::init');
