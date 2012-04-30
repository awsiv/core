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
            $jsonArray = $this->getResults('/promise/log/notkept/');
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
        $jsonArray = $this->getResults('/promise/log/notkept/?hostkey=' . $this->hostA);
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
        $jsonArray = $this->getResults('/promise/log/notkept?handle=' . $handle);
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
        $jsonArray = $this->getResults('/promise/log/notkept?context=' . $context);
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

    public function testPaginationEven()
    {
        $page = $this->getResults('/promise/log/notkept?count=2&page=1');
        $this->assertValidJson($page);
        $this->assertEquals(2, sizeof($page));
        $this->assertEquals(1327576540, $page[0]['timestamp']);
        $this->assertEquals(1327576540, $page[1]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=2&page=2');
        $this->assertValidJson($page);
        $this->assertEquals(2, sizeof($page));
        $this->assertEquals(1327576272, $page[0]['timestamp']);
        $this->assertEquals(1327576272, $page[1]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=2&page=3');
        $this->assertValidJson($page);
        $this->assertEquals(2, sizeof($page));
        $this->assertEquals(1327575944, $page[0]['timestamp']);
        $this->assertEquals(1327575944, $page[1]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=2&page=4');
        $this->assertValidJson($page);
        $this->assertEquals(2, sizeof($page));
        $this->assertEquals(1327575676, $page[0]['timestamp']);
        $this->assertEquals(1327575676, $page[1]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=2&page=5');
        $this->assertValidJson($page);
        $this->assertEquals(0, sizeof($page));
    }

    public function testPaginationOdd()
    {
        $page = $this->getResults('/promise/log/notkept?count=3&page=1');
        $this->assertValidJson($page);
        $this->assertEquals(3, sizeof($page));
        $this->assertEquals(1327576540, $page[0]['timestamp']);
        $this->assertEquals(1327576540, $page[1]['timestamp']);
        $this->assertEquals(1327576272, $page[2]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=3&page=2');
        $this->assertValidJson($page);
        $this->assertEquals(3, sizeof($page));
        $this->assertEquals(1327576272, $page[0]['timestamp']);
        $this->assertEquals(1327575944, $page[1]['timestamp']);
        $this->assertEquals(1327575944, $page[2]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=3&page=3');
        $this->assertValidJson($page);
        $this->assertEquals(2, sizeof($page));
        $this->assertEquals(1327575676, $page[0]['timestamp']);
        $this->assertEquals(1327575676, $page[1]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=3&page=4');
        $this->assertValidJson($page);
        $this->assertEquals(0, sizeof($page));
    }

    public function testPaginationSingle()
    {
        $page = $this->getResults('/promise/log/notkept?count=1&page=1');
        $this->assertValidJson($page);
        $this->assertEquals(1, sizeof($page));
        $this->assertEquals(1327576540, $page[0]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=1&page=2');
        $this->assertValidJson($page);
        $this->assertEquals(1, sizeof($page));
        $this->assertEquals(1327576540, $page[0]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=1&page=3');
        $this->assertValidJson($page);
        $this->assertEquals(1, sizeof($page));
        $this->assertEquals(1327576272, $page[0]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=1&page=4');
        $this->assertValidJson($page);
        $this->assertEquals(1, sizeof($page));
        $this->assertEquals(1327576272, $page[0]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=1&page=5');
        $this->assertValidJson($page);
        $this->assertEquals(1, sizeof($page));
        $this->assertEquals(1327575944, $page[0]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=1&page=6');
        $this->assertValidJson($page);
        $this->assertEquals(1, sizeof($page));
        $this->assertEquals(1327575944, $page[0]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=1&page=7');
        $this->assertValidJson($page);
        $this->assertEquals(1, sizeof($page));
        $this->assertEquals(1327575676, $page[0]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=1&page=8');
        $this->assertValidJson($page);
        $this->assertEquals(1, sizeof($page));
        $this->assertEquals(1327575676, $page[0]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=3&page=4');
        $this->assertValidJson($page);
        $this->assertEquals(0, sizeof($page));
    }

    public function testPaginationFull()
    {
        $page = $this->getResults('/promise/log/notkept?count=8&page=1');
        $this->assertValidJson($page);
        $this->assertEquals(8, sizeof($page));
        $this->assertEquals(1327576540, $page[0]['timestamp']);

        $page = $this->getResults('/promise/log/notkept?count=8&page=2');
        $this->assertValidJson($page);
        $this->assertEquals(0, sizeof($page));
    }
}
