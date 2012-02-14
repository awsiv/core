<?php

require_once "RestBaseTest.php";

class StatusTest extends RestBaseTest
{

    public function testVersionOne()
    {
        try
        {
            $jsonArray = $this->getResults('/');
            $this->assertEquals($jsonArray['api-version'], 'v1');
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

}
