<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";

class RestBaseTest extends PHPUnit_Framework_TestCase
{

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
    }

}
