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

    public function testAddSettingValidLdapEncryption()
    {
        try
        {
            $this->pest->post('/settings', '{
                "ldapEncryption": "ssl"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals('ssl', $settings[0]['ldapEncryption']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testAddSettingInvalidLdapEncryption()
    {
        try
        {
            $this->pest->post('/settings', '{
                "ldapEncryption": "turbo"
                }');
            $this->fail('Should not get here');
        }
        catch (Pest_NotAcceptable $e)
        {
            // pass
            return;
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
            $this->assertFalse($settings[0]['rbac']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdateSettingExternalAdminUsername()
    {
        try
        {
            // default external admin is 'admin'
            $settings = $this->getResults('/settings');
            $this->assertEquals(200, $this->pest->lastStatus());
            $this->assertValidJson($settings);
            $this->assertEquals('admin', $settings[0]['externalAdminUsername']);

            // try changing external admin to snookie
            $this->pest->post('/settings', '{
                "externalAdminUsername": "snookie"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());
            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals('snookie', $settings[0]['externalAdminUsername']);

            // change back to not confuse other tests
            $this->pest->post('/settings', '{
                "externalAdminUsername": "admin"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());
            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals('admin', $settings[0]['externalAdminUsername']);
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
                "ldapPort": 1025,
                "ldapPortSSL": 1026,
                "externalAdminUsername": "sitch"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals('yahoo.comxxx', $settings[0]['ldapHost']);
            $this->assertEquals('dc=cfengine;dc=com', $settings[0]['ldapBaseDN']);
            $this->assertEquals('uid', $settings[0]['ldapLoginAttribute']);
            $this->assertEquals('ou=jersey', $settings[0]['ldapUsersDirectory']);
            $this->assertEquals(1024, $settings[0]['ldapPort']);
            $this->assertEquals(1025, $settings[0]['ldapPortSSL']);
            $this->assertEquals('ssl', $settings[0]['ldapEncryption']);
            $this->assertEquals('ronnie', $settings[0]['externalAdminUsername']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

}
