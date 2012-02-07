<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";
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
