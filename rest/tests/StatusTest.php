<?php

require_once "RestBaseTest.php";

class StatusTest extends RestBaseTest
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

    public function testMetadata()
    {
        try
        {
            $jsonArray = $this->pest->get('/');
            $this->assertEquals(1, $jsonArray['meta']['count']);
            $this->assertEquals(1, $jsonArray['meta']['page']);
            $this->assertEquals(1, $jsonArray['meta']['total']);
            $this->assertGreaterThan(0, $jsonArray['meta']['timestamp']);
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
            $this->assertEquals('application/vnd.cfengine.nova-v1+json',
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
