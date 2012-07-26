<?php

require_once "APIBaseTest.php";

class SettingsTest extends APIBaseTest
{

    public function testGetSettings()
    {
        try
        {
            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals(1, sizeof($settings));

            $this->assertEquals("google.com", $settings[0]['ldapHost']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testAddSetting()
    {
        try
        {
            $this->pest->post('/settings', '{
                "ldapEncryption": "turbo"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals('turbo', $settings[0]['ldapEncryption']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdateSetting()
    {
        try
        {
            $this->pest->post('/settings', '{
                "ldapHost": "yahoo.com"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals('yahoo.com', $settings[0]['ldapHost']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdateSettingForBlueHostSetting()
    {
        try
        {
            $this->pest->post('/settings', '{
                "blueHostHorizon": 3600
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals(3600, $settings[0]['blueHostHorizon']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }
    
     public function testUpdateSettingRbac()
    {
        try
        {
            $this->pest->post('/settings', '{
                "rbac": false
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals(3600, $settings[0]['blueHostHorizon']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdateLdapSetting()
    {
        try
        {
            $this->pest->post('/settings', '{
                "ldapHost": "yahoo.comxxx",
                "ldapBaseDN":"dc=cfengine;dc=com",
                "ldapLoginAttribute":"uid",
                "ldapUsersDirectory":"ou=jersey",
                "ldapEncryption":"ssl",
                "authMode":"ldap"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals('ldap', $settings[0]['authMode']);
            $this->assertEquals('yahoo.comxxx', $settings[0]['ldapHost']);
            $this->assertEquals('dc=cfengine;dc=com', $settings[0]['ldapBaseDN']);
            $this->assertEquals('uid', $settings[0]['ldapLoginAttribute']);
            $this->assertEquals('ou=jersey', $settings[0]['ldapUsersDirectory']);
//$this->assertEquals('shore', $settings[0]['ldapUsersDirectory'][1]);
            $this->assertEquals('ssl', $settings[0]['ldapEncryption']);
//$this->assertEquals('ronnie', $settings[0]['externalAdmin']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testLdapBadLogin()
    {
        try
        {
            $this->pest->post('/settings', '{
                "ldapHost":"yahoo.comxxx",
                "ldapBaseDN":"dc=cfengine;dc=com",
                "ldapLoginAttribute":"uid",
                "ldapUsersDirectory":"ou=jersey",
                "ldapEncryption":"ssl",
                "authMode":"ldap"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $this->pest->setupAuth("ronnie", "password");
            $settings = $this->getResults('/');
            $this->fail("Should not reach");
        }
        catch (Pest_Unauthorized $e)
        {
            // pass
            return;
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }
    
    public function testLdapGoodLogin()
    {
        try
        {
            $this->pest->post('/settings', '{
                "ldapHost":"10.0.0.145",
                "ldapBaseDN":"dc=cf022osx;dc=cfengine;dc=com",
                "ldapLoginAttribute":"uid",
                "ldapUsersDirectory":"cn=users",
                "ldapEncryption":"ssl",
                "authMode":"ldap"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $this->pest->setupAuth("sudhir", "q1w2e3r4t5");
            $settings = $this->getResults('/');
            var_dump($settings);
            $this->assertValidJson($settings);
        }
        catch (Pest_Unauthorized $e)
        {
           $this->fail($e);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }
    
    

}
