<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";
require_once "RestBaseTest.php";

class PromiseLogNotKeptSummaryTest extends RestBaseTest {

    /**
     * test valid json response 
     */
    public function testAllPromiseLogNotKeptSummary() {
        try {
            $jsonArray = $this->pest->get('/promise/log/not-kept/summary');
            $this->assertValidJson($jsonArray);
        } catch (Pest_NotFound $e) {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test PromiseLogNotKept with host key parameter 
     */
    public function testPromiseLogNotKeptSummaryWithHostKey() {

        $hostKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $jsonArray = $this->pest->get('/promise/log/not-kept/summary?hostkey=' . $hostKey);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
    }

    /**
     * Test PromiseLogNotKept with handle 
     */
    public function testPromiseLogNotKeptSummaryWithHandle() {

        $handle = "cfengine_php_mod_files_cfmod_ini_ubuntu10";
        $jsonArray = $this->pest->get('/promise/log/not-kept/summary?handle=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertCount(1, $jsonArray);
        foreach ((array) $jsonArray as $data) {
            if ($data['handle'] !== "$handle") {
                $this->fail("different handle found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }
    
    
     /**
     * Test  PromiseLogNotKept with context 
     */
    public function testPromiseLogNotKeptSummaryWithContext() {

        $context = "10_0_0_153";
        $jsonArray = $this->pest->get('/promise/log/not-kept/summary?context=' . $context);
        $this->assertValidJson($jsonArray);
        $handle = "cdp_cmd_c__windows_system32_cmd_exe__c__echo_hello_succeeded___c__reportfile_txt___hello_failed";
        $this->assertFalse(empty($jsonArray),"Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if ($data['handle'] !== "$handle") {
                $this->fail("different handle  found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }
    
    
    

}
