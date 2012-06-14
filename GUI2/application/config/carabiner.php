<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

/**
 * Carabiner 1.45 configuration file.
 * CodeIgniter-library for Asset Management
 */
/*
  |--------------------------------------------------------------------------
  | Script Directory
  |--------------------------------------------------------------------------
  |
  | Path to the script directory.  Relative to the CI front controller.
  |
 */

$config['script_dir'] = 'scripts/';


/*
  |--------------------------------------------------------------------------
  | Style Directory
  |--------------------------------------------------------------------------
  |
  | Path to the style directory.  Relative to the CI front controller
  |
 */
if(is_constellation()){
  $config['style_dir'] = 'themes/constellation/css/';
}else{
  $config['style_dir'] = 'themes/default/css/';
}


/*
  |--------------------------------------------------------------------------
  | Cache Directory
  |--------------------------------------------------------------------------
  |
  | Path to the cache directory. Must be writable. Relative to the CI
  | front controller.
  |
 */

$config['cache_dir'] = 'tmp/';




/*
 * The following config values are not required.  See Libraries/Carabiner.php
 * for more information.
 */



/*
  |--------------------------------------------------------------------------
  | Base URI
  |--------------------------------------------------------------------------
  |
  |  Base uri of the site, like http://www.example.com/ Defaults to the CI
  |  config value for base_url.
  |
 */

//$config['base_uri'] = 'http://www.example.com/';


/*
  |--------------------------------------------------------------------------
  | Development Flag
  |--------------------------------------------------------------------------
  |
  |  Flags whether your in a development environment or not. Defaults to FALSE.
  |
 */

$config['dev'] = (ENVIRONMENT == 'development') ? true : false; // set in index.php


/*
  |--------------------------------------------------------------------------
  | Combine
  |--------------------------------------------------------------------------
  |
  | Flags whether files should be combined. Defaults to TRUE.
  |
 */

$config['combine'] = TRUE;


/*
  |--------------------------------------------------------------------------
  | Minify Javascript
  |--------------------------------------------------------------------------
  |
  | Global flag for whether JS should be minified. Defaults to TRUE.
  |
 */

$config['minify_js'] = TRUE;


/*
  |--------------------------------------------------------------------------
  | Minify CSS
  |--------------------------------------------------------------------------
  |
  | Global flag for whether CSS should be minified. Defaults to TRUE.
  |
 */

$config['minify_css'] = TRUE;

/*
  |--------------------------------------------------------------------------
  | Force cURL
  |--------------------------------------------------------------------------
  |
  | Global flag for whether to force the use of cURL instead of file_get_contents()
  | Defaults to FALSE.
  |
 */

$config['force_curl'] = FALSE;


/*
  |--------------------------------------------------------------------------
  | Predifined Asset Groups
  |--------------------------------------------------------------------------
  |
  | Any groups defined here will automatically be included.  Of course, they
  | won't be displayed unless you explicity display them ( like this: $this->carabiner->display('jquery') )
  | See docs for more.
  |
  | Currently created groups:
  |	> jQuery (latest in 1.xx version)
  |	> jQuery UI (latest in 1.xx version)
  |	> Ext Core (latest in 3.xx version)
  |	> Chrome Frame (latest in 1.xx version)
  |	> Prototype (latest in 1.x.x.x version)
  |	> script.aculo.us (latest in 1.x.x version)
  |	> Mootools (1.xx version)
  |	> Dojo (latest in 1.xx version)
  |	> SWFObject (latest in 2.xx version)
  |	> YUI (latest core JS/CSS in 2.x.x version)
  |
 */


// CFEngine base
$config['groups']['CFEngine'] = array(
    'css' => array(
        array('resets.css', 'screen'),
        array('Base.css', 'screen'),
        array('960.css', 'screen'),
        array('appstyle.css', 'screen'),
        array('finders.css','screen'),
        array('jquery-ui-1.8.10.custom.css', 'screen'),
        array('jquery.qtip.css', 'screen')
    ),
    'js' => array(
        array('jquery-1.7.1.min.js', 'jquery-1.7.1.min.js', TRUE, FALSE),
        array('jquery-ui-1.8.18.custom.min.js', 'jquery-ui-1.8.18.custom.min.js', TRUE, FALSE),
        array('jquery.qtip.min.js', 'jquery.qtip.min.js', TRUE, FALSE),
        array('dateformat.js', 'dateformat.js', TRUE, FALSE),
        array('common.js', 'common.js', TRUE, FALSE),
    )
);


$config['groups']['CFEngineEditor'] = array(
    'css' => array(
        array('resets.css', 'screen'),
        array('Base.css', 'screen'),
        array('960.css', 'screen'),
        array('jquery-ui-1.8.10.custom.css', 'screen'),
        array('cfeditor.css', 'screen'),
        array('jquery.jnotify.css', 'screen')
    ),
    'js' => array(
        array('jquery-1.7.1.min.js', 'jquery-1.7.1.min.js', FALSE),
        array('jquery-ui-1.8.18.custom.min.js', 'jquery-ui-1.8.18.custom.min.js', FALSE),
        array('Cfeditor/jquery.layout.min-1.2.0.js','Cfeditor/jquery.layout.min-1.2.0.js'),
        array('jquery.tablesorter.min.js','jquery.tablesorter.min.js'),
        array('jqueryFileTree.js','jqueryFileTree.js'),
        array('jquery.jnotify.min.js','jquery.jnotify.min.js'),
        array('widgets/notes.js','widgets/notes.js'),
        array('jquery.jcryption-1.1.min.js','jquery.jcryption-1.1.min.js'),
        array('Cfeditor/lib/codemirror.js','Cfeditor/lib/codemirror.js',FALSE),
        array('Cfeditor/lib/shortcut.js','Cfeditor/lib/shortcut.js'),
        array('Cfeditor/mode/cfengine/cfsyntax.js','Cfeditor/mode/cfengine/cfsyntax.js'),
        array('Cfeditor/mode/cfengine/cfengine.js','Cfeditor/mode/cfengine/cfengine.js',FALSE),
        array('cfeditor.js','cfeditor.js'),
        array('Cfeditor/mode/cfengine/cfautocomplete.js','Cfeditor/mode/cfengine/cfautocomplete.js')
    )
);

