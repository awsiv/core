<?php if (!defined('BASEPATH')) exit('No direct script access allowed');
	
/*
|--------------------------------------------------------------------------
| application settings
|
| 'image dir' = image directory path to be used by application.
| 'scripts dir' = scripts directory path where the  java scripts are placed .
|--------------------------------------------------------------------------
*/
$config['image_dir'] = 'images/';
$config['css_dir'] = 'css/';
$config['scripts_dir']= 'scripts/';
$config['hub_dir']='/hub/';
$config['graph_dir']='./graphs/';
$config['policies_dir']='./policies/';

/*
|----------------------------------------------------------------------------
|This item is for adjusting the environment on which the cfengine web
|app is deployed. On production it should be stated as production
|---------------------------------------------------------------------------
*/
$config['env']='development';