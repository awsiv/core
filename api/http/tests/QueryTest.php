<?php

require_once "APIBaseTest.php";

class QueryTest extends APIBaseTest
{
    public function testHostsTableQuery(){
         try
        {
            $response = $this->pest->post('/query', 'SELECT * FROM Hosts ORDER BY HostKey');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            // todo check result meta

            // check result table schema 
            $this->assertEquals('HostKey', $response['data'][0]['header'][0]);
            $this->assertEquals('Name', $response['data'][0]['header'][1]);
            $this->assertEquals('IPAddress', $response['data'][0]['header'][2]);
            $this->assertEquals('ReportTimeStamp', $response['data'][0]['header'][3]);
            $this->assertEquals('Colour', $response['data'][0]['header'][4]);

            // check result row count
            $this->assertEquals('2', $response['data'][0]['row_count']);

            // check result rows content
            //row 1
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][0][0]);
            $this->assertEquals('hostA', $response['data'][0]['rows'][0][1]);
            $this->assertEquals('10.0.0.150', $response['data'][0]['rows'][0][2]);
            $this->assertEquals('1327651818', $response['data'][0]['rows'][0][3]);
            $this->assertEquals('blue', $response['data'][0]['rows'][0][4]);

            // row 2
            $this->assertEquals('33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856', $response['data'][0]['rows'][1][0]);
            $this->assertEquals('hostB', $response['data'][0]['rows'][1][1]);
            $this->assertEquals('10.0.0.153', $response['data'][0]['rows'][1][2]);
            $this->assertEquals('1328194213', $response['data'][0]['rows'][1][3]);
            $this->assertEquals('blue', $response['data'][0]['rows'][1][4]);
 
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testFileChangesTableQuery(){
         try
        {
            $response = $this->pest->post('/query', 'SELECT * FROM FileChanges ORDER BY HostKey');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            // todo check result meta

            // check result table schema 
            $this->assertEquals('HostKey', $response['data'][0]['header'][0]);
            $this->assertEquals('FileName', $response['data'][0]['header'][1]);
            $this->assertEquals('ChangeTimeStamp', $response['data'][0]['header'][2]);

            // check result row count
            $this->assertEquals('2', $response['data'][0]['row_count']);

            // check result rows content
            //row 1
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][0][0]);
            $this->assertEquals('/etc/group', $response['data'][0]['rows'][0][1]);
            $this->assertEquals('1234567', $response['data'][0]['rows'][0][2]);

            // row 2
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][1][0]);
            $this->assertEquals('/etc/passwd', $response['data'][0]['rows'][1][1]);
            $this->assertEquals('1234567', $response['data'][0]['rows'][1][2]);
 
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testContextsTableQuery(){
         try
        {
            $response = $this->pest->post('/query', 'SELECT * FROM Contexts ORDER BY HostKey LIMIT 2');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            // todo check result meta

            // check result table schema 
            $this->assertEquals('HostKey', $response['data'][0]['header'][0]);
            $this->assertEquals('Name', $response['data'][0]['header'][1]);
            $this->assertEquals('DefineTimeStamp', $response['data'][0]['header'][2]);

            // check result row count
            $this->assertEquals('2', $response['data'][0]['row_count']);

            // check result rows content
            //row 1
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][0][0]);
            $this->assertEquals('x86_64', $response['data'][0]['rows'][0][1]);
            $this->assertEquals('1327653377', $response['data'][0]['rows'][0][2]);

            // row 2
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][1][0]);
            $this->assertEquals('ubuntu_10_4', $response['data'][0]['rows'][1][1]);
            $this->assertEquals('1327653377', $response['data'][0]['rows'][1][2]);
 
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }
}
