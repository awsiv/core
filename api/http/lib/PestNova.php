<?php

class PestNova extends Pest
{
  protected function prepRequest($opts, $url) {
    return parent::prepRequest($opts, $url);
  }

  public function processBody($body) {
    return json_decode($body, true);
  }
}