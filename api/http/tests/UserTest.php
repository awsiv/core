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

    public function testAddDeleteUser()
    {
        try
        {
            // add new user
            $this->pest->put('/user/snookie', '{
                    "password": "pass"
                }');
            $this->assertEquals(201, $this->pest->lastStatus());

            // check user was added
            $users = $this->getResults('/user');
            $this->assertValidJson($users);
            $this->assertEquals('snookie', $users[0]['username']);

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
