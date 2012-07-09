<?php

require_once "RestBaseTest.php";

class ContextTest extends RestBaseTest
{

    public function testAllContext()
    {
        try
        {
            $jsonArray = $this->getResults('/context');
            $this->assertValidJson($jsonArray);
            $this->assertFalse(empty($jsonArray), "Should not be empty value");
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testContextByName()
    {
        try
        {
            $jsonArray = $this->getResults('/context?name=io_writtendata_.*' . $ip);
            $this->assertValidJson($jsonArray);
            $this->assertEquals(2, sizeof($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

}
