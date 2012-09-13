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

            $this->assertEquals(5, sizeof($promises));
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }
}