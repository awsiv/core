<?php
class test_goals_model extends CodeIgniterUnitTestCase {
    private $username="admin";
    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('goals_model');
    }

    public function tearDown() {
        
    }
    
     public function test_getAllGoals(){
         $goals = $this->_ci->goals_model->getAllGoals();
         $this->assertTrue(is_array($goals),"Array must be returned");
         $this->dump($goals); 
    }
}
?>
