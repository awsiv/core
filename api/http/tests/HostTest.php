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

    public function testListAllContextsForHost()
    {
        try
        {
            $response = $this->pest->get('/host/' . $this->hostA_id . '/context');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);
            $this->assertEquals(111, sizeof($response['data']));
            $this->assertEquals(111, $response['meta']['total']);
            $this->assertEquals(111, $response['meta']['count']);
            $this->assertEquals('x86_64', $response['data'][0]['id']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testListHostsByContextIncludeSingle()
    {
        try
        {
            $response = $this->pest->get('/host?context-include=PK_SHA_305.*');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);
            $this->assertEquals(1, sizeof($response['data']));
            $this->assertEquals(1, $response['meta']['total']);
            $this->assertEquals(1, $response['meta']['count']);
            $this->assertEquals($this->hostA_id, $response['data'][0]['id']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testListHostsByContextExcludeSingle()
    {
        try
        {
            $response = $this->pest->get('/host?context-exclude=PK_SHA_305.*');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);
            $this->assertEquals(1, sizeof($response['data']));
            $this->assertEquals(1, $response['meta']['total']);
            $this->assertEquals(1, $response['meta']['count']);
            $this->assertEquals($this->hostB_id, $response['data'][0]['id']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testListVitals()
    {
        try
        {
            $response = $this->pest->get('/host/' . $this->hostA_id . '/vital');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);
            $this->assertEquals(3, sizeof($response['data']));
            $this->assertEquals(3, $response['meta']['total']);
            $this->assertEquals(3, $response['meta']['count']);
            $this->assertEquals('io_reads', $response['data'][0]['id']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testGetVital()
    {
        try
        {
            $from = 1327493194;
            $to = 1327579594;

            $response = $this->pest->get('/host/' . $this->hostA_id . '/vital/io_reads?from=' . $from . '&to=' . $to);
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);
            $this->assertEquals(1, sizeof($response['data']));
            $this->assertEquals(1, $response['meta']['total']);
            $this->assertEquals(1, $response['meta']['count']);

            $vital = $response['data'][0];
            $this->assertEquals('io_reads', $vital['id']);
            $this->assertEquals(288, sizeof($vital['values']));

            foreach ($vital['values'] as $point)
            {
                $this->assertLessThanOrEqual($to, $point[0]);
                $this->assertGreaterThanOrEqual($from, $point[0]);
            }
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testGetVitalCorrectTimestamp()
    {
        try
        {
            $response = $this->pest->get('/host/' . $this->hostA_id . '/vital/test');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);
            $this->assertEquals(1, sizeof($response['data']));
            $this->assertEquals(1, $response['meta']['total']);
            $this->assertEquals(1, $response['meta']['count']);

            $vital = $response['data'][0];
            $this->assertEquals('test', $vital['id']);
            $this->assertEquals(1, sizeof($vital['values']));
            $this->assertEquals(1327276800, $vital['values'][0][0]);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testDeleteHost()
    {
        try
        {
            $hosts = $this->getResults('/host');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($hosts);
            $this->assertEquals(2, sizeof($hosts));

            $this->pest->delete('/host/' . $this->hostA_id);
            $this->assertEquals(204, $this->pest->lastStatus());

            $hosts = $this->getResults('/host');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($hosts);
            $this->assertEquals(1, sizeof($hosts));
            $this->assertEquals($this->hostB_id, $hosts[0]['id']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }
}
