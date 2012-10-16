<?php

require_once "PHPUnit/Autoload.php";
require_once __DIR__ . "/../lib/Pest.php";
require_once __DIR__ . "/../lib/PestNova.php";

abstract class APIBaseTest extends PHPUnit_Framework_TestCase
{

    public $hostA_id = "305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283";
    public $hostA_ip = "10.0.0.150";
    public $hostB_id = "33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856";
    public $hostB_ip = "10.0.0.153";
    var $mongoInstance = null;
    public $ldapSettings = '{
                "ldapEnabled": true,
                "ldapMode": "standard",
                "ldapHost": "localhost",
                "ldapPort": 1025,
                "ldapBaseDN": "dc=localhost",
                "ldapLoginAttribute": "uid",
                "ldapUsersDirectory": "ou=people",
                "ldapEncryption": "plain",
                "ldapUsername": "snookie",
                "ldapPassword": "pass"
                }';

    public function __construct()
    {

        $this->mongoInstance = new Mongo("localhost:27777");
        parent::__construct();
    }

    function assertValidJson($data)
    {
        if ($data === null && json_last_error() !== JSON_ERROR_NONE)
        {
            $this->fail("Bad json input string");
        }
        return $data;
    }

    function array_keys_exists($array, $keys)
    {
        foreach ($keys as $k)
        {
            if (!isset($array[$k]))
            {
                return false;
            }
        }
        return true;
    }

    public function setUp()
    {
        $this->baseUrl = "http://localhost:8888/api";
        $this->pest = null;
        $this->pest = new PestNova($this->baseUrl);
        $this->pest->setupAuth("admin", "admin");

        $this->loadFixtures();
    }

    public function getResults($path)
    {
        $jsonArray = $this->pest->get($path);
        $this->assertValidJson($jsonArray);
        return $jsonArray['data'];
    }

    public function addHeader($header)
    {
        $this->pest->curl_opts[CURLOPT_HTTPHEADER][] = $header;
    }

    public function tearDown()
    {
        $database = array('cfreport', 'phpcfengine');
        foreach ($database as $db)
        {
            $mongodb = $this->mongoInstance->$db;
            $mongodb->drop();
        }
    }

    public function loadFixtures()
    {
        {
            $fixtures = array(
                'settings' => 'cfreport',
                'roles' => 'cfreport',
                'users' => 'cfreport',
                'hosts' => 'cfreport',
                'logs_rep' => 'cfreport',
                'logs_nk' => 'cfreport',
                'archive' => 'cfreport',
                'monitoring_mg' => 'cfreport',
                'promises_unexp' => 'cfreport'
            );

            foreach ($fixtures as $collection => $db)
            {
                $mongodb = $this->mongoInstance->$db;
                $mongoCollection = $mongodb->$collection;
                $fileName = __DIR__ . "/fixtures/" . $db . "." . $collection . '.json';
                $fileContent = file_get_contents($fileName);


                $c = json_decode($fileContent, true);
                if (!is_array($c))
                {
                    echo ('Json decode failed for  ' . $collection . ' ');
                }
                $insertData = $c;
                $mongoCollection->drop();
                $res = $mongoCollection->batchInsert($insertData, array('continueOnError' => false, 'fsync' => true));
            }
        }
    }

}
