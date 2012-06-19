<?php

// Errors on full!
ini_set('display_errors', 1);
error_reporting(E_ALL | E_STRICT);

$dir = realpath(dirname(__FILE__));

// Path constants
defined('PROJECT_BASE') OR define('PROJECT_BASE', realpath($dir.'/../').'/');
defined('BASEPATH') OR define('BASEPATH', PROJECT_BASE.'system/');
defined('APPPATH') OR define('APPPATH', PROJECT_BASE.'application/');
defined('VIEWPATH') OR define('VIEWPATH', PROJECT_BASE.'');
define('EXT', '.php');
define('HUB_MASTER', 'localhost');
define('ENVIRONMENT', 'testing');
 $mongoPort = getenv('CFENGINE_TEST_OVERRIDE_MONGO_PORT');

if (!$mongoPort) {
    define('MONGO_PORT', '27777');
} else {
     define('MONGO_PORT', $mongoPort);
}



// Get vfsStream either via PEAR or composer
foreach (explode(PATH_SEPARATOR, get_include_path()) as $path)
{
	if (file_exists($path.DIRECTORY_SEPARATOR.'vfsStream/vfsStream.php'))
	{
		require_once 'vfsStream/vfsStream.php';
		break;
	}
}

if ( ! class_exists('vfsStream') && file_exists(PROJECT_BASE.'vendor/autoload.php'))
{
	include_once PROJECT_BASE.'vendor/autoload.php';
	class_alias('org\bovigo\vfs\vfsStream', 'vfsStream');
	class_alias('org\bovigo\vfs\vfsStreamDirectory', 'vfsStreamDirectory');
	class_alias('org\bovigo\vfs\vfsStreamWrapper', 'vfsStreamWrapper');
}

// Prep our test environment
include_once $dir.'/mocks/core/common.php';
include_once $dir.'/mocks/autoloader.php';
spl_autoload_register('autoload');

unset($dir);