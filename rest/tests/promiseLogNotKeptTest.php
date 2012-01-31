<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";
require_once "restBase.php";

class promiseLogNotKeptTest extends RestBaseTest {

    /**
     * test valid json response 
     */
    public function testAllPromiseLogNotKept() {
        try {
            $jsonArray = $this->pest->get('/promise/log/not-kept/');
            $this->assertValidJson($jsonArray);
        } catch (Pest_NotFound $e) {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test PromiseLogNotKept with host key parameter 
     */
    public function testPromiseLogNotKeptWithHostKey() {

        $hostkey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $jsonArray = $this->pest->get('/promise/log/not-kept/?hostkey=' . $hostkey);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
        foreach ((array) $jsonArray as $data) {
            if ($data['hostkey'] !== "$hostkey") {
                $this->fail("different hostkey  found in data, found :: " . $data['hostkey'] . " Expected :: " . $hostkey);
            }
        }
    }

    /**
     * Test PromiseLogNotKept with handle 
     */
    public function testPromiseLogNotKeptWithHandle() {

        $handle = "cfengine_php_mod_files_cfmod_ini_ubuntu10";
        $jsonArray = $this->pest->get('/promise/log/not-kept?handle=' . $handle);
        $this->assertValidJson($jsonArray);
        foreach ((array) $jsonArray as $data) {
            if ($data['handle'] !== "$handle") {
                $this->fail("different handle found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }
    
    
     /**
     * Test  PromiseLogNotKept with context 
     */
    public function testPromiseLogNotKeptWithContext() {

        $context = "10_0_0_153";
        $jsonArray = $this->pest->get('/promise/log/not-kept?context=' . $context);
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
