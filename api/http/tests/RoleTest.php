<?php

require_once "APIBaseTest.php";

class RoleTest extends APIBaseTest
{
    public function testListToles()
    {
        try
        {
            $jsonArray = $this->getResults('/role');
            $this->assertValidJson($jsonArray);
            $this->assertEquals(1, sizeof($jsonArray));
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }

    public function testAddRole(){
        try{
         $this->pest->put('/role/jersey', '{
                    "description": "Jersey Shore Role",
                     "classrxinclude":"linux",
                     "classrxexclude":"windows",
                     "bundlerxinlcude":"bundle_1",
                     "bundlerxexclude":"bundle_2"
                }');
            $this->assertEquals(201, $this->pest->lastStatus());

            // check role was added
            $roles = $this->getResults('/role');
            $this->assertValidJson($roles);
            $this->assertEquals('jersey', $roles[1]['name']);
            $this->assertEquals('linux', $roles[1]['classrxinclude']);
            $this->assertEquals('windows', $roles[1]['classrxexclude']);
            $this->assertEquals('bundle_1', $roles[1]['bundlerxinlcude']);
            $this->assertEquals('bundle_2', $roles[1]['bundlerxexclude']);
             
        }catch(Pest_Exception $e)
        {
            $this->fail($e); 
        }
    }
    
    public function testEditRole(){
         try
        {
            // add new role
            $this->pest->post('/role/jersey', '{
                    "description": "Jersey Shore modified",
                    "crxi":"linux,windows"
                }');
            $this->assertEquals(204, $this->pest->lastStatus());

            // check role was added
            $roles = $this->getResults('/role');
            $this->assertValidJson($roles);
            $this->assertEquals('jersey', $roles[1]['name']);
            $this->assertEquals('Jersey Shore modified', $roles[1]['description']);
        }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }
    
    public function testDeleteRole(){
         // delete role
        try{
            $this->pest->delete('/role/jersey');
            $this->assertEquals(204, $this->pest->lastStatus());
             }
        catch (Pest_Exception $e)
        {
            $this->fail($e);
        }
    }
    
}
