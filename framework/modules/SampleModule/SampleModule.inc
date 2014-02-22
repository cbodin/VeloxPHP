<?php
Module::loadInclude('SampleModule', 'SampleModuleActions.inc');

class SampleModule {
  static function frontPage() {
    return 'lolsan';
  }
}
