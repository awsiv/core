<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";
require_once "RestBaseTest.php";

class promiseLogRepairedTest extends RestBaseTest {

    /**
     * test valid json response 
     */
    public function testAllPromiseLogRepaired() {
        try {
            $jsonArray = $this->pest->get('/promise/log/repaired/');
            $this->assertValidJson($jsonArray);
        } catch (Pest_NotFound $e) {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test Promise Log Repaired   with host key parameter 
     */
    public function testPromiseLogRepairedWithHostKey() {

        $hostKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $jsonArray = $this->pest->get('/promise/log/repaired?hostkey=' . $hostKey);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
    }

    /**
     * Test Promise Log Repaired summary with handle 
     */
    public function testPromiseLogRepairedWithHandle() {

        $handle = "cfengine_php_mod_commands_apache2_debian";
        $jsonArray = $this->pest->get('/promise/log/repaired?handle=' . $handle);
        $this->assertValidJson($jsonArray);
        foreach ((array) $jsonArray as $data) {
            if ($data['handle'] !== "$handle") {
                $this->fail("different handle found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }
    
    
     /**
     * Test  Promise Log Repaired summary with context 
     */
    public function testPromiseLogRepairedWithContext() {

        $context = "10_0_0_153";
        $jsonArray = $this->pest->get('/promise/log/repaired?context=' . $context);
        $this->assertValidJson($jsonArray);
        $hostkey = "SHA=33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856";
        $this->assertFalse(empty($jsonArray),"Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if ($data['hostkey'] !== "$hostkey") {
                $this->fail("different hostkey  found in data, found :: " . $data['hostkey'] . " Expected :: " . $hostkey);
            }
        }
    }    

}
