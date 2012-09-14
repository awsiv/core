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
                "rbacEnabled": false
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertFalse($settings[0]['rbacEnabled']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdateLdapSettings()
    {
        try
        {
            $this->pest->post('/settings', '{
                "ldapMode": "standard",
                "ldapHost": "yahoo.comxxx",
                "ldapBaseDN":"dc=cfengine;dc=com",
                "ldapLoginAttribute":"uidx",
                "ldapUsersDirectory":"ou=jersey",
                "ldapEncryption":"ssl",
                "ldapPort": 1025,
                "ldapPortSSL": 1026,
                "ldapUsername": "root",
                "ldapPassword": "secret"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals(false, $settings[0]['ldapEnabled']);
            $this->assertEquals('standard', $settings[0]['ldapMode']);
            $this->assertEquals('yahoo.comxxx', $settings[0]['ldapHost']);
            $this->assertEquals('dc=cfengine;dc=com', $settings[0]['ldapBaseDN']);
            $this->assertEquals('uidx', $settings[0]['ldapLoginAttribute']);
            $this->assertEquals('ou=jersey', $settings[0]['ldapUsersDirectory']);
            $this->assertEquals(1025, $settings[0]['ldapPort']);
            $this->assertEquals(1026, $settings[0]['ldapPortSSL']);
            $this->assertEquals('ssl', $settings[0]['ldapEncryption']);
            $this->assertEquals('root', $settings[0]['ldapUsername']);
            $this->assertEquals('secret', $settings[0]['ldapPassword']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdateLdapSettingNoPassword()
    {
        try
        {
            $this->pest->post('/settings', '{
                "ldapEnabled": true,
                "ldapMode": "standard",
                "ldapHost": "yahoo.comxxx",
                "ldapBaseDN":"dc=cfengine;dc=com",
                "ldapLoginAttribute":"uid",
                "ldapUsersDirectory":"ou=jersey",
                "ldapEncryption":"ssl",
                "ldapPort": 1025,
                "ldapPortSSL": 1026,
                "ldapUsername": "root",
                }');

            $this->fail('Should not get here');
        }
        catch (Pest_NotAcceptable $e)
        {
            //pass
        }
        catch (Pest_Exception $e)
        {
            $this->fail('Invalid reply: ' . $this->pest->lastStatus());
        }
    }

    public function testUpdateSwitchLDAPMode()
    {
        try
        {
            // here
            $this->pest->post('/settings', '{
                "ldapMode": "standard",
                }');
            $this->assertEquals(204, $this->pest->lastStatus());
            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals('standard', $settings[0]['ldapMode']);

            // there
            $this->pest->post('/settings', '{
                "ldapMode": "activeDirectory",
                }');
            $this->assertEquals(204, $this->pest->lastStatus());
            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals('activeDirectory', $settings[0]['ldapMode']);

            // and back again
            $this->pest->post('/settings', '{
                "ldapMode": "standard",
                }');
            $this->assertEquals(204, $this->pest->lastStatus());
            $settings = $this->getResults('/settings');
            $this->assertValidJson($settings);
            $this->assertEquals('standard', $settings[0]['ldapMode']);


        }
        catch (Pest_Exception $e)
        {
            $this->fail('Invalid reply: ' . $this->pest->lastStatus());
        }
    }
}
