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

}
