<?php

require_once "RestBaseTest.php";

class SoftwareTest extends RestBaseTest
{

    /**
     * test valid json response 
     */
    public function testAll()
    {
        try
        {
            $jsonArray = $this->pest->get('/software');
            $this->assertValidJson($jsonArray);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test software with host key 
     */
    public function testWithHostKey()
    {
        $jsonArray = $this->pest->get('/software?hostkey=' . $this->hostA);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
        foreach ((array) $jsonArray as $data)
        {
            if (!in_array($this->hostA, $data['hostkeys']))
            {
                $this->fail("No hostkey found in data, Expected :: " . $this->hostA);
            }
        }
    }

    /**
     * Test software with name 
     */
    public function testWithName()
    {

        $handle = "apparmor";
        $jsonArray = $this->pest->get('/software?name=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if ($data['name'] !== "$handle")
            {
                $this->fail("different path found in data, found :: " . $data['name'] . " Expected :: " . $handle);
            }
        }
    }

    /**
     * Test software with context 
     */
    public function testWithContext()
    {

        $context = "10_0_0_150";
        $jsonArray = $this->pest->get('/software?context=' . $context);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if (!in_array($this->hostA, $data['hostkeys']))
            {
                $this->fail("No hostkey found in data, Expected :: " . $this->hostA);
            }
        }
    }

    /**
     * Test software with name 
     */
    public function testWithEmptyArchValue()
    {

        $handle = "apparmor";
        $jsonArray = $this->pest->get('/software?name=' . $handle);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if ($data['name'] !== "$handle")
            {
                $this->fail("different path found in data, found :: " . $data['name'] . " Expected :: " . $handle);
            }
        }
    }

    /**
     * Test software with version 
     */
    public function testWithVersion()
    {

        $version = "3.112ubuntu1";
        $jsonArray = $this->pest->get('/software?version=' . $version);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        $handle = "adduser";
        foreach ((array) $jsonArray as $data)
        {
            if ($data['name'] !== "$handle")
            {
                $this->fail("different name found in data, found :: " . $data['name'] . " Expected :: " . $handle);
            }
        }
    }

    public function testWithDefaultArch()
    {

        $arch = "default";
        $jsonArray = $this->pest->get('/software?arch=' . $arch);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if ($data['arch'] !== "$arch")
            {
                $this->fail("different name found in data, found :: " . $data['arch'] . " Expected :: " . $arch);
            }
        }
    }

    public function testWithInvalidArch()
    {
        $jsonArray = $this->pest->get('/software?arch=snookie');
        $this->assertTrue(empty($jsonArray));
    }
    
    public function testWithX_86_64Arch()
    {

        $arch = "x86_64";
        $jsonArray = $this->pest->get('/software?arch=' . $arch);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if ($data['arch'] !== "$arch")
            {
                $this->fail("different name found in data, found :: " . $data['arch'] . " Expected :: " . $arch);
            }
        }
    }

}
