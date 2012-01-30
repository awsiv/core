<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";
require_once "restBase.php";

class softwareTest extends restBaseTest {

    /**
     * test valid json response 
     */
    public function testAllSoftware() {
        try {
            $jsonArray = $this->pest->get('/software');
            $this->assertValidJson($jsonArray);
        } catch (Pest_NotFound $e) {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test software with host key 
     */
    public function testSoftwareWithHostKey() {

        $hostKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $jsonArray = $this->pest->get('/software?hostkey=' . $hostKey);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
        foreach ((array) $jsonArray as $data) {
            if (!in_array($hostKey,$data['hostkeys'])) {
                $this->fail("No hostkey found in data, Expected :: " . $hostKey);
            }
        }
    }

    /**
     * Test software with name 
     */
    public function testSoftwareWithName() {

        $handle = "apache2";
        $jsonArray = $this->pest->get('/software?name=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if ($data['name'] !== "$handle") {
                $this->fail("different path found in data, found :: " . $data['name'] . " Expected :: " . $handle);
            }
        }
    }

    /**
     * Test software with context 
     */
    public function testSoftwareWithContext() {

        $context = "10_0_0_29";
        $jsonArray = $this->pest->get('/software?context=' . $context);
        $this->assertValidJson($jsonArray);
        $hostKey = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df";
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if (!in_array( $hostKey,$data['hostkeys'])) {
                $this->fail("No hostkey found in data, Expected :: " . $hostKey);
            }
        }
    }
    
    /**
     *  Test the failing data for software
     */
     /**
     * Test software with name 
     */
    public function testSoftwareWithEmptyArchValue() {

        $handle = "Deployment_Guide-en-US";
        $jsonArray = $this->pest->get('/software?name=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if ($data['name'] !== "$handle") {
                $this->fail("different path found in data, found :: " . $data['name'] . " Expected :: " . $handle);
            }
        }
    }
    
    
    /**
     * Test software with version 
     */
    public function testSoftwareWithVersion() {

        $version = "3.23-1.2";
        $jsonArray = $this->pest->get('/software?version=' . $version);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        $handle = "MAKEDEV";
        foreach ((array) $jsonArray as $data) {
            if ($data['name'] !== "$handle") {
                $this->fail("different name found in data, found :: " . $data['name'] . " Expected :: " . $handle);
            }
        }
    }
    
    public function testSoftwareWithDefaultArch() {

        $arch = "default";
        $jsonArray = $this->pest->get('/software?arch=' . $arch);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if ($data['arch'] !== "$arch") {
                $this->fail("different name found in data, found :: " . $data['arch'] . " Expected :: " . $arch);
            }
        }
    }
    
     public function testSoftwareWithX_86_64Arch() {

        $arch = "x_86_64";
        $jsonArray = $this->pest->get('/software?arch=' . $arch);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data) {
            if ($data['arch'] !== "$arch") {
                $this->fail("different name found in data, found :: " . $data['arch'] . " Expected :: " . $arch);
            }
        }
    }
    
    

}
