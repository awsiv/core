<?php

require_once "RestBaseTest.php";

class BaseTest extends RestBaseTest
{

    public function testVersionOne()
    {
        try
        {
            $jsonArray = $this->pest->get('/');
            $this->assertValidJson($jsonArray);
            $this->assertEquals($jsonArray['api-version'], 'v1');
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

}
