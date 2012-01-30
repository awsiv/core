<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";
require_once "restBase.php";

class hostIdTest extends restBaseTest {

    /**
     * Checks for valid json response 
     */
    public function testhostID() {
        try {
            $id = "SHA=eb98cd79ff30c793a815ab4f29dc5d1400fc45a13f30db69bedb4c8ca3ab8576";
            $jsonArray = $this->pest->get("/host/$id");
            $this->assertValidJson($jsonArray);
        } catch (Pest_NotFound $e) {
            $this->fail('Resource not found');
        }
    }

     /**
     * 
     * Matches the data format with values that are returned and expected 
     */
    public function testhostIdData() {
        try {
            $id = "SHA=eb98cd79ff30c793a815ab4f29dc5d1400fc45a13f30db69bedb4c8ca3ab8576";
            $jsonArray = $this->pest->get("/host/$id");
            $this->assertValidJson($jsonArray);
            $expectedArray = array(
                "hostkey" => "SHA=eb98cd79ff30c793a815ab4f29dc5d1400fc45a13f30db69bedb4c8ca3ab8576",
                "ip" => "10.0.0.156",
                "name" => "suse1.test.cfengine.com"
            );
            $this->assertTrue($jsonArray == $expectedArray,"Expected values and returned array values are different.");
        } catch (Pest_NotFound $e) {
            $this->fail('Resource not found');
        }
    }
    
    /**
     * This muust throw exception of resource not found
     * @expectedException Pest_NotFound 
     */
     public function testhostIdWithInvalidData() {       
            $id = "SHA=eb98cd79ff30c793a815ab4f29dc5d1400fc45a13f30db69bedb4c8ca3ab8579";
            $this->pest->get("/host/$id");           
    }

}
