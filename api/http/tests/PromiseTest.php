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
}