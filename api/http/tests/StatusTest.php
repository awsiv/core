<?php

require_once "APIBaseTest.php";

class StatusTest extends APIBaseTest
{
    public function testVersionOne()
    {
        try
        {
            $jsonArray = $this->getResults('/');
            $this->assertEquals('v1', $jsonArray[0]['apiVersion']);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testContentType()
    {
        try
        {
            $jsonArray = $this->pest->get('/');
            $this->assertEquals('application/vnd.cfengine.enterprise-v1+json',
                    $this->pest->last_response['meta']['content_type']);
        }
        catch (Pest_NotFound $e)
        {
            $this->fail('Resource not found');
        }
    }

    public function testUnsupportedAccept()
    {
        try
        {
            $this->addHeader('Accept: application/xml');
            $jsonArray = $this->pest->get('/');
        }
        catch (Pest_ClientError $e)
        {
            $this->assertEquals(406, $this->pest->lastStatus());
            return;
        }
        $this->fail('Should not pass');
    }

    public function testBadAuth()
    {
        try
        {
            $this->pest->setupAuth('snookie', 'sitch');
            $jsonArray = $this->pest->get('/');
        }
        catch (Pest_Unauthorized $e)
        {
            return;
        }
        $this->fail('Should not pass');
    }
}
