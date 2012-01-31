<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";
require_once "restBase.php";

class promiseLogRepairedSummaryTest extends RestBaseTest {

    /**
     * test valid json response 
     */
    public function testAllPromiseLogRepairedSummary() {
        try {
            $jsonArray = $this->pest->get('/promise/log/repaired/summary');
            $this->assertValidJson($jsonArray);
        } catch (Pest_NotFound $e) {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test Promise Log Repaired summary  with host key parameter 
     */
    public function testPromiseLogRepairedSummaryWithHostKey() {

        $hostKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $jsonArray = $this->pest->get('/promise/log/repaired/summary?hostkey=' . $hostKey);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
    }

    /**
     * Test Promise Log Repaired summary with handle 
     */
    public function testPromiseLogRepairedSummaryWithHandle() {

        $handle = "cfengine_php_mod_commands_apache2_debian";
        $jsonArray = $this->pest->get('/promise/log/repaired/summary?handle=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertCount(1, $jsonArray);
        foreach ((array) $jsonArray as $data) {
            if ($data['handle'] !== "$handle") {
                $this->fail("different handle found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }
    
    
     /**
     * Test  Promise Log Repaired summary with context 
     */
    public function testPromiseLogRepairedSummaryWithContext() {

        $context = "10_0_0_153";
        $jsonArray = $this->pest->get('/promise/log/repaired/summary?context=' . $context);
        $this->assertValidJson($jsonArray);
        $handle = "garbage_collection_files_tidy_outputs";
        $this->assertFalse(empty($jsonArray),"Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if ($data['handle'] !== "$handle") {
                $this->fail("different handle  found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }    

}
