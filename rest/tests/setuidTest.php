<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";
require_once "restBase.php";

class setUidTest extends RestBaseTest {

    /**
     * test valid json response 
     */
    public function testAllSetUid() {
        try {
            $jsonArray = $this->pest->get('/setuid');
            $this->assertValidJson($jsonArray);
        } catch (Pest_NotFound $e) {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test setuid with host key 
     */
    public function testSetUidWithHostKey() {

        $hostKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $jsonArray = $this->pest->get('/setuid?hostkey=' . $hostKey);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
        foreach ((array) $jsonArray as $data) {
            if (!in_array($hostKey,$data['hostkeys'])) {
                $this->fail("No hostkey found in data, Expected :: " . $hostKey);
            }
        }
    }

    /**
     * Test setuid with name 
     */
    public function testSetUidWithName() {

        $handle = "/usr/bin/chfn";
        $jsonArray = $this->pest->get('/setuid?name=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if ($data['path'] !== "$handle") {
                $this->fail("different path found in data, found :: " . $data['path'] . " Expected :: " . $handle);
            }
        }
    }

    /**
     * Test setuid with context 
     */
    public function testSetUidWithContext() {

        $context = "10_0_0_29";
        $jsonArray = $this->pest->get('/setuid?context=' . $context);
        $this->assertValidJson($jsonArray);
        $hostKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if (!in_array( $hostKey,$data['hostkeys'])) {
                $this->fail("No hostkey found in data, Expected :: " . $hostKey);
            }
        }
    }

}
