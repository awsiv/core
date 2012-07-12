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


    public function testAddUser(){
        try{
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

            //test newly created user can log in or not
            $this->pest->setupAuth("snookie", "pass");
            $response=$this->getResults('');
            $this->assertValidJson($response);

        }catch(Exception $e){
            $this->fail($e);
        }
    }


    public function testUpdateEmail(){
        try{
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
            $this->getResults('');
            $this->assertValidJson($response);

        }catch(Exception $e){
            $this->fail($e);
        }
    }

    public function testUpdateRoles(){
        try{
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
            $response=$this->getResults('');
            $this->assertValidJson($response);

        }catch(Exception $e){
              $this->fail($e);
         }
    }

    public function testBrowseOwnDetails(){
        try{
          $this->pest->setupAuth("snookie", "pass");
          $users=$this->getResults('/user/snookie');
          $this->assertEquals(200, $this->pest->lastStatus());
          $this->assertEquals('snookie', $users[0]['username']);

         }catch(Exception $e){
            $this->fail($e);
          }

    }

    public function testChangePassword(){
     try{
          //change password
          $this->pest->post('/user/snookie', '{
                    "password": "pass2"
                }');
          $this->assertEquals(204, $this->pest->lastStatus());

          //check if authentication is successful for new password
          $this->pest->setupAuth("snookie", "pass2");
          $response=$this->getResults('');
          $this->assertValidJson($response);

          //check if roles are still there
          $users = $this->getResults('/user/snookie');
          $this->assertEquals('jersey', $users[0]['roles'][0]);
          $this->assertEquals('wenches', $users[0]['roles'][1]);

        }catch(Exception $e){
             $this->fail($e);
        }
    }

    public function testDeleteUser(){
        try{
            $this->pest->delete('/user/snookie');
            $this->assertEquals(204, $this->pest->lastStatus());
        }catch(Exception $e){
               $this->fail($e);
        }
    }


    public function testUserNotFound()
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
}
