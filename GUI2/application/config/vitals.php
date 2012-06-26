<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');
/**
 * Predifined values for vital selection in vital tabs
 */
$config['vitals'] = array(
    'loadavg' => array("text" => "load average", 'max' => 2, 'min' => 0),
    'diskfree' => array("text" => "Disk free", 'max' => 100, 'min' => 0),
    'cpu' => array("text" => "CPU(ALL)", 'max' => 100)
);

$config['vitals_max_nodes'] = 200;
