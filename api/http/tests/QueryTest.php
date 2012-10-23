<?php

require_once "APIBaseTest.php";

class QueryTest extends APIBaseTest
{

    public function testHostsTableQuery()
    {
        try
        {
            $response = $this->pest->post('/query', '{
                "query": "SELECT * FROM Hosts ORDER BY HostKey"
                }');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            // todo check result meta

            $this->assertEquals(4, sizeof($response['data'][0]['header']));

            // check result table schema
            $this->assertEquals('HostKey', $response['data'][0]['header'][0]);
            $this->assertEquals('HostName', $response['data'][0]['header'][1]);
            $this->assertEquals('IPAddress', $response['data'][0]['header'][2]);
            $this->assertEquals('ReportTimeStamp', $response['data'][0]['header'][3]);

            // check result row count
            $this->assertEquals('2', $response['data'][0]['rowCount']);

            // check result rows content
            //row 1
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][0][0]);
            $this->assertEquals('hostA', $response['data'][0]['rows'][0][1]);
            $this->assertEquals('10.0.0.150', $response['data'][0]['rows'][0][2]);
            $this->assertEquals('1327651818', $response['data'][0]['rows'][0][3]);

            // row 2
            $this->assertEquals('33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856', $response['data'][0]['rows'][1][0]);
            $this->assertEquals('hostB', $response['data'][0]['rows'][1][1]);
            $this->assertEquals('10.0.0.153', $response['data'][0]['rows'][1][2]);
            $this->assertEquals('1328194213', $response['data'][0]['rows'][1][3]);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testFileChangesTableQuery()
    {
        try
        {
            $response = $this->pest->post('/query', '{
                "query": "SELECT * FROM FileChanges ORDER BY HostKey"
                }');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            // todo check result meta
            // check result table schema
            $this->assertEquals('HostKey', $response['data'][0]['header'][0]);
            $this->assertEquals('FileName', $response['data'][0]['header'][1]);
            $this->assertEquals('ChangeTimeStamp', $response['data'][0]['header'][2]);

            // check result row count
            $this->assertEquals('2', $response['data'][0]['rowCount']);

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

    public function testContextsTableQuery()
    {
        try
        {
            $response = $this->pest->post('/query', '{
                "query": "SELECT * FROM Contexts ORDER BY HostKey LIMIT 2"
                }');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            // todo check result meta
            // check result table schema
            $this->assertEquals('HostKey', $response['data'][0]['header'][0]);
            $this->assertEquals('ContextName', $response['data'][0]['header'][1]);
            $this->assertEquals('DefineTimeStamp', $response['data'][0]['header'][2]);

            // check result row count
            $this->assertEquals('2', $response['data'][0]['rowCount']);

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

    public function testVariablesTableQuery()
    {
        try
        {
            $response = $this->pest->post('/query', '{
                "query": "' . "SELECT * FROM Variables WHERE VariableType='string' OR VariableType='string list' ORDER BY VariableType LIMIT 4" . '"' .
                    '}');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            // todo check result meta
            // check result table schema
            $this->assertEquals('HostKey', $response['data'][0]['header'][0]);
            $this->assertEquals('NameSpace', $response['data'][0]['header'][1]);
            $this->assertEquals('Bundle', $response['data'][0]['header'][2]);
            $this->assertEquals('VariableName', $response['data'][0]['header'][3]);
            $this->assertEquals('VariableValue', $response['data'][0]['header'][4]);
            $this->assertEquals('VariableType', $response['data'][0]['header'][5]);

            // check result row count
            $this->assertEquals('4', $response['data'][0]['rowCount']);

            // check result rows content
            //row 1
            $this->assertEquals('33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856', $response['data'][0]['rows'][0][0]);
            $this->assertEquals('', $response['data'][0]['rows'][0][1]);
            $this->assertEquals('control_reporter', $response['data'][0]['rows'][0][2]);
            $this->assertEquals('style_sheet', $response['data'][0]['rows'][0][3]);
            $this->assertEquals('/cf_enterprise.css', $response['data'][0]['rows'][0][4]);
            $this->assertEquals('string', $response['data'][0]['rows'][0][5]);

            //row 2
            $this->assertEquals('33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856', $response['data'][0]['rows'][1][0]);
            $this->assertEquals('', $response['data'][0]['rows'][0][1]);
            $this->assertEquals('control_reporter', $response['data'][0]['rows'][1][2]);
            $this->assertEquals('build_directory', $response['data'][0]['rows'][1][3]);
            $this->assertEquals('/var/cfengine/reports', $response['data'][0]['rows'][1][4]);
            $this->assertEquals('string', $response['data'][0]['rows'][1][5]);

            //row 3
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][2][0]);
            $this->assertEquals('', $response['data'][0]['rows'][0][1]);
            $this->assertEquals('sys', $response['data'][0]['rows'][2][2]);
            $this->assertEquals('ostype', $response['data'][0]['rows'][2][3]);
            $this->assertEquals('ubuntu_10_0_4_x86', $response['data'][0]['rows'][2][4]);
            $this->assertEquals('string', $response['data'][0]['rows'][2][5]);

            //row 4
            $this->assertEquals('33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856', $response['data'][0]['rows'][3][0]);
            $this->assertEquals('', $response['data'][0]['rows'][0][1]);
            $this->assertEquals('control_runagent', $response['data'][0]['rows'][3][2]);
            $this->assertEquals('hosts', $response['data'][0]['rows'][3][3]);
            $this->assertEquals('127.0.0.1', $response['data'][0]['rows'][3][4]);
            $this->assertEquals('string list', $response['data'][0]['rows'][3][5]);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testSoftwareTableQuery()
    {
        try
        {
            $response = $this->pest->post('/query', '{
                "query": "SELECT * FROM Software ORDER BY SoftwareName LIMIT 4"
                }');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            // todo check result meta
            // check result table schema
            $this->assertEquals('HostKey', $response['data'][0]['header'][0]);
            $this->assertEquals('SoftwareName', $response['data'][0]['header'][1]);
            $this->assertEquals('SoftwareVersion', $response['data'][0]['header'][2]);
            $this->assertEquals('SoftwareArchitecture', $response['data'][0]['header'][3]);

            // check result row count
            $this->assertEquals('4', $response['data'][0]['rowCount']);

            // check result rows content
            //row 1
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][0][0]);
            $this->assertEquals('adduser', $response['data'][0]['rows'][0][1]);
            $this->assertEquals('3.112ubuntu1', $response['data'][0]['rows'][0][2]);
            $this->assertEquals('default', $response['data'][0]['rows'][0][3]);

            //row 2
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][1][0]);
            $this->assertEquals('apparmor', $response['data'][0]['rows'][1][1]);
            $this->assertEquals('2.5.1-0ubuntu0.10.04.3', $response['data'][0]['rows'][1][2]);
            $this->assertEquals('default', $response['data'][0]['rows'][1][3]);

            //row 3
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][2][0]);
            $this->assertEquals('apparmor-utils', $response['data'][0]['rows'][2][1]);
            $this->assertEquals('2.5.1-0ubuntu0.10.04.3', $response['data'][0]['rows'][2][2]);
            $this->assertEquals('default', $response['data'][0]['rows'][2][3]);

            //row 4
            $this->assertEquals('33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856', $response['data'][0]['rows'][3][0]);
            $this->assertEquals('cyrus-sasl-lib', $response['data'][0]['rows'][3][1]);
            $this->assertEquals('2.1.22-5.el5_4.3', $response['data'][0]['rows'][3][2]);
            $this->assertEquals('x86_64', $response['data'][0]['rows'][3][3]);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testPromiseStatusLastTableQuery()
    {
        try
        {
            $response = $this->pest->post('/query', '{
                "query": "SELECT * FROM  PromiseStatusLast ORDER BY PromiseHandle LIMIT 2"
                }');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            // todo check result meta
            // check result table schema
            $this->assertEquals('HostKey', $response['data'][0]['header'][0]);
            $this->assertEquals('PromiseHandle', $response['data'][0]['header'][1]);
            $this->assertEquals('PromiseStatus', $response['data'][0]['header'][2]);
            $this->assertEquals('CheckTimeStamp', $response['data'][0]['header'][3]);

            // check result row count
            $this->assertEquals('2', $response['data'][0]['rowCount']);

            // check result rows content
            //row 1
            $this->assertEquals('33736d45041e2a9407be8cf449aeffa95114bef661c20deaca1bbcfbc2922856', $response['data'][0]['rows'][0][0]);
            $this->assertEquals('cdp_cmd_c__windows_system32_cmd_exe__c__echo_hello_succeeded___c__reportfile_txt___hello_failed', $response['data'][0]['rows'][0][1]);
            $this->assertEquals('notkept', $response['data'][0]['rows'][0][2]);
            $this->assertEquals('1328194134', $response['data'][0]['rows'][0][3]);

            //row 2
            $this->assertEquals('305658693b94e003e765956f1609731419cbc0e5c9caa09e230df5e005f1f283', $response['data'][0]['rows'][1][0]);
            $this->assertEquals('cdp_cmd_c__windows_system32_cmd_exe__c__echo_hello_succeeded___c__reportfile_txt___hello_failed', $response['data'][0]['rows'][1][1]);
            $this->assertEquals('notkept', $response['data'][0]['rows'][1][2]);
            $this->assertEquals('1327653377', $response['data'][0]['rows'][1][3]);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testHostsTableQueryAsync()
    {
        try
        {
            $response = $this->pest->post('/query/async', '{
                "query": "SELECT * FROM Hosts ORDER BY HostKey"
                }');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($response);

            $token = $response['data'][0]['id'];
            var_dump($response);
            $this->assertNotNull($token);

            $response = $this->pest->get('/query/async/' . $token);
            $this->assertEquals(200, $this->pest->lastStatus());

            $response = $this->pest->delete('/query/async/' . $token);
            $this->assertEquals(204, $this->pest->lastStatus());
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

}
