<?php

require_once "PHPUnit/Autoload.php";
require_once __DIR__ . "/../lib/Pest.php";
require_once __DIR__ . "/../lib/PestNova.php";

abstract class APIBaseTest extends PHPUnit_Framework_TestCase
{
    public $hostA = "SHA=305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283";
    public $hostA_ip = "10.0.0.150";

    public $hostB = "SHA=33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856";
    public $hostB_ip = "10.0.0.153";

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
        $this->baseUrl = "http://localhost:8888/api";
        $this->pest = null;
        $this->pest = new PestNova($this->baseUrl);
        $this->pest->setupAuth("admin", "admin");
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
}
