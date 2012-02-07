<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";
require_once "RestBaseTest.php";

class PromiseComplianceTest extends RestBaseTest {

    /**
     * test valid json response 
     */
    public function testAllPromiseCompliance() {
        try {
            $jsonArray = $this->pest->get('/promise/compliance');
            $this->assertValidJson($jsonArray);
        } catch (Pest_NotFound $e) {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test promise compliance with host key parameter 
     */
    public function testPromiseComplianceWithHostKey() {

        $hostKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $jsonArray = $this->pest->get('/promise/compliance?hostkey=' . $hostKey);
        $this->assertValidJson($jsonArray);
        foreach ((array) $jsonArray as $data) {
            if ($data['hostkey'] !== "$hostKey") {
                $this->fail("different host key found in data found :: " . $data['hostkey'] . " Expected :: " . $hostKey);
            }
        }
    }

    /**
     * Test promise compliance with handle 
     */
    public function testPromiseComplianceWithHandle() {

        $handle = "internal_promise";
        $jsonArray = $this->pest->get('/promise/compliance?handle=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertCount(1, $jsonArray);
        foreach ((array) $jsonArray as $data) {
            if ($data['handle'] !== "$handle") {
                $this->fail("different handle found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }
    
    
     /**
     * Test promise compliance with context 
     */
    public function testPromiseComplianceWithContext() {

        $context = "10_0_0_153";
        $jsonArray = $this->pest->get('/promise/compliance?context=' . $context);
        $this->assertValidJson($jsonArray);
        $hostKey = "SHA=33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856";
        $this->assertFalse(empty($jsonArray),"Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if ($data['hostkey'] !== "$hostKey") {
                $this->fail("different host key found in data, found :: " . $data['hostkey'] . " Expected :: " . $hostKey);
            }
        }
    }
    
    
     /**
     * Test promise compliance with state 
     */
    public function testPromiseComplianceWithState() {

        $state = "state";
        $jsonArray = $this->pest->get('/promise/compliance?state=' . $state);
        $this->assertValidJson($jsonArray);
        $this->fail('Not Implemented, parameter not considered');
    }

}
