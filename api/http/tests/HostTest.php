<?php

require_once "APIBaseTest.php";

class HostTest extends APIBaseTest
{
    public function testListAllHosts()
    {
        try
        {
            $hosts = $this->getResults('/host');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($hosts);
            $this->assertEquals(2, sizeof($hosts));
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testListAllHostsPagination()
    {
        try
        {
            $response = $this->pest->get('/host?page=1&count=1');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            $this->assertEquals(2, $response['meta']['total']);
            $this->assertEquals(1, $response['meta']['count']);
            $this->assertEquals(1, $response['meta']['page']);

            $this->assertValidJson($response['data']);
            $this->assertEquals(1, sizeof($response['data']));
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testGetHost()
    {
        try
        {
            $hosts = $this->getResults('/host/' . $this->hostA_id);
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($hosts);
            $this->assertEquals(1, sizeof($hosts));

            $this->assertEquals($this->hostA_id, $hosts[0]['id']);
            $this->assertEquals('hostA', $hosts[0]['hostname']);
            $this->assertEquals($this->hostA_ip, $hosts[0]['ip']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

}
