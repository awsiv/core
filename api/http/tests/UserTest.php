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

    // not really a "unit" test :)
    public function testAddUpdateDeleteUser()
    {
        try
        {
            // add new user
            $this->pest->put('/user/snookie', '{
                    "password": "pass",
                    "active": true,
                    "email": "snookie@cfengine.com"
                }');
            $this->assertEquals(201, $this->pest->lastStatus());

            // check user was added
            $users = $this->getResults('/user');
            $this->assertValidJson($users);
            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals(true, $users[0]['active']);
            $this->assertEquals('snookie@cfengine.com', $users[0]['email']);

            // update email
            $this->pest->post('/user/snookie', '{
                    "email": "snookie2@cfengine.com"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            // check email was updated
            $users = $this->getResults('/user');
            $this->assertValidJson($users);
            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals('snookie2@cfengine.com', $users[0]['email']);

            // update deactivate
            $this->pest->post('/user/snookie', '{
                    "active": false
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            // check email was updated
            $users = $this->getResults('/user');
            $this->assertValidJson($users);
            $this->assertEquals('snookie', $users[0]['username']);
            $this->assertEquals(false, $users[0]['active']);

            // delete user
            $this->pest->delete('/user/snookie');
            $this->assertEquals(204, $this->pest->lastStatus());
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }
}
