<?php

require_once "APIBaseTest.php";

class PromiseTest extends APIBaseTest
{
    public function testListAll()
    {
        try
        {
            $promises = $this->getResults('/promise');
            $this->assertEquals(200, $this->pest->lastStatus());

            $this->assertEquals(7, sizeof($promises));
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testListByType()
    {
        try
        {
            $promises = $this->getResults('/promise?type=classes');
            $this->assertEquals(200, $this->pest->lastStatus());

            $this->assertEquals(3, sizeof($promises));
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testGet()
    {
        try
        {
            $promises = $this->getResults('/promise/promise_promises_cf_122');
            $this->assertEquals(200, $this->pest->lastStatus());

            $this->assertEquals(1, sizeof($promises));
            $this->assertEquals('promise_promises_cf_122', $promises[0]['id']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }
}