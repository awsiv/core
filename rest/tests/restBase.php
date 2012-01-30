<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";

class restBaseTest extends PHPUnit_Framework_TestCase {

    function assertValidJson($data) {
        if ($data === null && json_last_error() !== JSON_ERROR_NONE) {
            $this->fail("Bad json string input string.");
        }
        return $data;
    }

    public function setUp() {

        $this->baseUrl = "http://rest.no-ip.biz:8888";
        $this->pest = new PestJSON($this->baseUrl);
    }
}
