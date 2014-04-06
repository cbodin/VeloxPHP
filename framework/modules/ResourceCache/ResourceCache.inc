<?php
Module::loadInclude('ResourceCache', 'classes/ResourceCache.inc');
Module::loadInclude('ResourceCache', 'classes/ResourceCacheActions.inc');

// Set the weight so that preprocessor modules can be run before and
// possibly preprocess less, sass, coffeescript etc.
Module::addActionCallback('js', 'ResourceCacheActions::js', 100);
Module::addActionCallback('css', 'ResourceCacheActions::css', 100);
