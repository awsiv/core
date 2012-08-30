<?php

require_once "APIBaseTest.php";

class UserTest extends APIBaseTest
{

    public function testListUsers()
    {
        try
        {
            $jsonArray = $this->getResults('/user');
            $this->assertValidJson($jsonArray);
            $this->assertEquals(1, sizeof($jsonArray));
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testAddUser()
    {
        try
        {
            $this->pest->put('/user/snookie', '{
                    "password": "pass",
                    "email": "snookie@cfengine.com",
                    "roles": [ "jersey" ]
                }');
            $this->assertEquals(201, $this->pest->lastStatus());

            // check user was added
            $users = $this->getResults('/user');
            $this->assertValidJson($users);
            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals('snookie@cfengine.com', $users[0]['email']);
            $this->assertEquals('jersey', $users[0]['roles'][0]);
            $this->assertEquals(false, $users[0]['external']);

            //test newly created user can log in or not
            $this->pest->setupAuth("snookie", "pass");
            $response = $this->getResults('');
            $this->assertValidJson($response);
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdateUnknownUser()
    {
        try
        {
            $this->pest->post('/user/snookie', '{
                    "email": "snookie2@cfengine.com"
                }');
            $this->fail('Should not get here');
        }
        catch (Pest_NotFound $e)
        {
            // pass
            return;
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdateEmail()
    {
        try
        {
            $this->pest->put('/user/snookie', '{
                    "password": "pass",
                    "email": "snookie@cfengine.com",
                    "roles": [ "jersey" ]
                }');
            $this->assertEquals(201, $this->pest->lastStatus());

            $this->pest->post('/user/snookie', '{
                    "email": "snookie2@cfengine.com"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            // check only email was updated and nothing other
            $users = $this->getResults('/user');
            $this->assertValidJson($users);
            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals('snookie2@cfengine.com', $users[0]['email']);

            //test if only email was edited
            $this->pest->setupAuth("snookie", "pass");
            $response = $this->getResults('');
            $this->assertValidJson($response);
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdateRoles()
    {
        try
        {
            $this->pest->put('/user/snookie', '{
                    "password": "pass",
                    "email": "snookie@cfengine.com",
                    "roles": [ "jersey" ]
                }');
            $this->assertEquals(201, $this->pest->lastStatus());

            $this->pest->post('/user/snookie', '{
                    "roles": [ "jersey", "wenches" ]
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            // check roles was updated
            $users = $this->getResults('/user');
            $this->assertValidJson($users);
            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals('jersey', $users[0]['roles'][0]);
            $this->assertEquals('wenches', $users[0]['roles'][1]);

            //test only roles was edited
            $this->pest->setupAuth("snookie", "pass");
            $response = $this->getResults('');
            $this->assertValidJson($response);
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

     public function testEmptyUserRoles()
    {
        try
        {
            $this->pest->put('/user/snookie', '{
                    "password": "pass",
                    "email": "snookie@cfengine.com",
                    "roles": [ "jersey" ]
                }');
            $this->assertEquals(201, $this->pest->lastStatus());

            $this->pest->post('/user/snookie', '{
                    "roles": []
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            // check roles was updated
            $users = $this->getResults('/user');
            $this->assertValidJson($users);
            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals(0, sizeof($users[0]['roles']));

            //test only roles was edited
            $this->pest->setupAuth("snookie", "pass");
            $response = $this->getResults('');
            $this->assertValidJson($response);
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testBrowseOwnDetails()
    {
        try
        {
            $this->pest->put('/user/snookie', '{
                    "password": "pass",
                    "email": "snookie@cfengine.com",
                    "roles": [ "jersey" ]
                }');

            $this->assertEquals(201, $this->pest->lastStatus());

            $this->pest->setupAuth("snookie", "pass");
            $users = $this->getResults('/user/snookie');

            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals(false, $users[0]['external']);
            $this->assertEquals('snookie@cfengine.com', $users[0]['email']);
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testChangePassword()
    {
        try
        {
            $this->pest->put('/user/snookie', '{
                    "password": "pass",
                    "email": "snookie@cfengine.com",
                    "roles": [ "jersey" ]
                }');

            $this->assertEquals(201, $this->pest->lastStatus());

            //change password
            $this->pest->post('/user/snookie', '{
                    "password": "pass2"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            //check if authentication is successful for new password
            $this->pest->setupAuth("snookie", "pass2");
            $response = $this->getResults('');
            $this->assertValidJson($response);

            //check if roles are still there
            $users = $this->getResults('/user/snookie');
            $this->assertEquals('jersey', $users[0]['roles'][0]);
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testDeleteUser()
    {
        try
        {
            $this->pest->put('/user/snookie', '{
                    "password": "pass",
                    "email": "snookie@cfengine.com",
                    "roles": [ "jersey" ]
                }');

            $this->assertEquals(201, $this->pest->lastStatus());

            $this->pest->delete('/user/snookie');
            $this->assertEquals(204, $this->pest->lastStatus());
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testRandomUserLogin()
    {
        try
        {
            $this->pest->setupAuth("asdasd", "asdasdsa");
            $this->getResults('');
            $this->fail("Should not reach");
        }
        catch (Pest_Unauthorized $e)
        {
            // pass
            return;
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUserNotFoundAsAdmin()
    {
        try
        {
            $jsonArray = $this->getResults('/user/xxxxxxx');
            $this->fail("Should not get here, user found");
        }
        catch (Pest_NotFound $e)
        {
            // pass
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testLDAPEvilAuthenticate()
    {
        try
        {
            $this->pest->post('/settings', $this->ldapSettings);
            $this->assertEquals(204, $this->pest->lastStatus());

            $this->pest->setupAuth("wario", "pass");
            $this->getResults('');
            $this->fail('should not get here');
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

    public function testLDAPListAsAdmin()
    {
        try
        {
            $this->pest->post('/settings', $this->ldapSettings);
            $this->assertEquals(204, $this->pest->lastStatus());

            $jsonArray = $this->getResults('/user');
            $this->assertValidJson($jsonArray);
            $this->assertEquals(3, sizeof($jsonArray));
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testLDAPListAsUser()
    {
        try
        {
            $this->pest->post('/settings', $this->ldapSettings);
            $this->assertEquals(204, $this->pest->lastStatus());

            $this->pest->setupAuth("snookie", "pass");
            $this->getResults('/user');
            $this->fail('should not get here');
        }
        catch (Pest_Forbidden $e)
        {
            // pass
            return;
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdateEmailForLDAPUserAsUser()
    {
        try
        {
            $this->pest->post('/settings', $this->ldapSettings);
            $this->assertEquals(204, $this->pest->lastStatus());

            $this->pest->setupAuth("snookie", "pass");
            $users = $this->getResults('/user/snookie');
            $this->assertValidJson($response);
            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals(null, $users[0]['email']);
            $this->assertEquals(true, $users[0]['external']);

            $this->pest->post('/user/snookie', '{
                    "email": "snookie2@cfengine.com"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            // check only email was updated and nothing other
            $users = $this->getResults('/user/snookie');
            $this->assertValidJson($response);
            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals('snookie2@cfengine.com', $users[0]['email']);
            $this->assertEquals(true, $users[0]['external']);

            //test if only email was edited
            $this->pest->setupAuth("snookie", "pass");
            $response = $this->getResults('');
            $this->assertValidJson($response);
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testUpdatePasswordFailsForLDAPUserAsUser()
    {
        try
        {
            $this->pest->post('/settings', $this->ldapSettings);
            $this->assertEquals(204, $this->pest->lastStatus());

            $this->pest->setupAuth("snookie", "pass");
            $users = $this->getResults('/user/snookie');
            $this->assertValidJson($response);
            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals(null, $users[0]['email']);
            $this->assertEquals(true, $users[0]['external']);

            $this->pest->post('/user/snookie', '{
                    "password": "wtf"
                }');
            $this->fail('Got return code: ' . $this->pest->lastStatus());
        }
        catch (Pest_Forbidden $e)
        {
            // pass
            return;
        }
        catch (Exception $e)
        {
            $this->fail($e);
        }
    }

}
