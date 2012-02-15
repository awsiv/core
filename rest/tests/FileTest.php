<?php

require_once "RestBaseTest.php";

class FileTest extends RestBaseTest
{

    public function testAll()
    {
        try
        {
            $jsonArray = $this->getResults('/file');
            $this->assertFalse(empty($jsonArray), "Should not be empty value");
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }
    
    public function testDiff()
    {
        try
        {
            $jsonArray = $this->getResults('/file?path=/etc/group');
            $diff = $jsonArray[0]['diff'];
            $this->assertEquals('add', $diff['type']);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

}
