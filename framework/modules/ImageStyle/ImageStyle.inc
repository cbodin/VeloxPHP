<?php
Module::loadInclude('ImageStyle', 'classes/ImageStyle.inc');
Module::addActionCallback('routeMatch', 'ImageStyle::checkImageStyleMatch');
