<?php

require_once "PHPUnit/Autoload.php";
require_once __DIR__ . "/../lib/Pest.php";
require_once __DIR__ . "/../lib/PestJSON.php";

class RestBaseTest extends PHPUnit_Framework_TestCase
{
    public $hostA = "SHA=305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283";
    public $hostB = "SHA=33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856";
    
    function assertValidJson($data)
    {
        if ($data === null && json_last_error() !== JSON_ERROR_NONE)
        {
            $this->fail("Bad json string input string.");
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
        $this->baseUrl = "http://localhost:8888/rest";
        $this->pest = new PestJSON($this->baseUrl);
        $this->pest->setupAuth("admin", "admin");
    }

}
