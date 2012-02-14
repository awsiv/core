<?php

require_once "RestBaseTest.php";

class PromiseLogNotKeptTest extends RestBaseTest
{

    /**
     * test valid json response 
     */
    public function testAllPromiseLogNotKept()
    {
        try
        {
            $jsonArray = $this->getResults('/promise/log/not-kept/');
            $this->assertValidJson($jsonArray);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test PromiseLogNotKept with host key parameter 
     */
    public function testWithHostKey()
    {
        $jsonArray = $this->getResults('/promise/log/not-kept/?hostkey=' . $this->hostA);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray));
        foreach ((array) $jsonArray as $data)
        {
            if ($data['hostkey'] !== $this->hostA)
            {
                $this->fail("different hostkey  found in data, found :: " . $data['hostkey'] . " Expected :: " . $this->hostA);
            }
        }
    }

    /**
     * Test PromiseLogNotKept with handle 
     */
    public function testWithHandle()
    {

        $handle = "cfengine_php_mod_files_cfmod_ini_ubuntu10";
        $jsonArray = $this->getResults('/promise/log/not-kept?handle=' . $handle);
        $this->assertValidJson($jsonArray);
        foreach ((array) $jsonArray as $data)
        {
            if ($data['handle'] !== "$handle")
            {
                $this->fail("different handle found in data, found :: " . $data['handle'] . " Expected :: " . $handle);
            }
        }
    }

    /**
     * Test  PromiseLogNotKept with context 
     */
    public function testWithContext()
    {

        $context = "10_0_0_150";
        $jsonArray = $this->getResults('/promise/log/not-kept?context=' . $context);
        $this->assertValidJson($jsonArray);
        $this->assertFalse(empty($jsonArray), "Should not return empty result.");
        foreach ((array) $jsonArray as $data)
        {
            if ($data['hostkey'] !== $this->hostA)
            {
                $this->fail("different hostkey  found in data, found :: " . $data['hostkey'] . " Expected :: " . $this->hostA);
            }
        }
    }

}
