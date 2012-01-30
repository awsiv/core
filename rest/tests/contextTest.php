<?php

require_once "PHPUnit/Autoload.php";
require_once "../lib/Pest.php";
require_once "../lib/PestJSON.php";
require_once "restBase.php";

class contextTest extends restBaseTest {

    public function testAllContext() {
        try {
            $jsonArray = $this->pest->get('/context');
            $this->assertValidJson($jsonArray);
            $this->assertFalse(empty($jsonArray),"Should not be empty value");
        } catch (Pest_NotFound $e) {
            $this->fail('Resource not found');
        }
    }

   

}
