<?php

require_once "RestBaseTest.php";

class HostIdSeenbyTest extends RestBaseTest
{

    /**
     * Test if the returned data is correct
     */
    public function testhostIdSeenBy()
    {
        try
        {
            $response = $this->pest->get('/host/' . $this->hostB . '/seen-by');
            $this->assertValidJson($response);
            $this->assertValidJson($response['data']);
            $this->assertEquals(1, $response['meta']['total']);
            $this->assertEquals($response['meta']['count'], sizeof($response['data']));
            $this->assertEquals(1, sizeof($response['data']));
            $this->assertEquals("SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697ca213df",
                    $response['data'][0]['hostkey']);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    /**
     * Test for the invalid key
     */
    public function testhostIdSeenByWithInvalidKey()
    {
        try
        {
            $id = "SHA=bd6dfcc28b1a7be234a68e3fe77e3c199e68fc28f400de0f94eadf697c";
            $jsonArray = $this->getResults("/host/$id/seen");
            $this->assertValidJson($jsonArray);
            $this->assertTrue(empty($jsonArray));
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }
}
