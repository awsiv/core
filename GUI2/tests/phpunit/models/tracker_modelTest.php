<?php

class test_tracker_model extends CI_TestCase
{

    private $ci_obj;
    private $username = "admin";

    public function set_up()
    {
        $config = $this->getMock('CI_Config', NULL, array(), '', FALSE);
        
         $this->ci_set_config(array(
            'index_page' => 'index.php',
            'base_url' => 'http://example.com/',
            'subclass_prefix' => 'Cf_'
        ));
        // Instantiate a new loader
        $this->load = new Mock_Core_Loader();
        // mock up a ci instance
        $this->ci_obj = new StdClass;
  


        // Fix get_instance()
        $this->ci_instance($this->ci_obj);
        $this->ci_instance_var('config', $config);

        $this->load->model('tracker_model');
        $this->load->library('mongo_db');
        
        $username='unit-test';
        $resource='handle';
        $reportType='reportType';
        $dateTime=time();
        $frequency=5;
        
        $data= array(
               'userName'=>$username,
               'resource'=>$resource,
               'reportType'=>$reportType,
               'dateTimeStamp'=>$dateTime,
               'frequency'=>$frequency,
               'trackerName'=>'testTracker'
        );
        $tracker= $this->ci_obj->tracker_model->insert($data);
    }
    
     public function tearDown()
     {
        // delete all the test inserts
        $filter = array('userName' => 'unit-test');
        $return = $this->ci_obj->tracker_model->delete($filter);
     }
     
     public function test_insertTracker(){
        $username='unit-test';
        $resource='handle';
        $reportType='reportType';
        $dateTime=time();
        $frequency=5;
        
        $data= array(
               'userName'=>$username,
               'resource'=>$resource,
               'reportType'=>$reportType,
               'dateTimeStamp'=>$dateTime,
               'frequency'=>$frequency,
               'trackerName'=>'testTracker1'
        );
        $tracker= $this->ci_obj->tracker_model->insert($data);
        $this->assertInstanceOf('CF_tracker', $tracker);
        $this->assertEquals($tracker->getUserName(), 'unit-test');
     }
     
      public function test_insertduplicateTrackerMustFail(){
        $username='unit-test';
        $resource='handle';
        $reportType='reportType';
        $dateTime=time();
        $frequency=5;
        
        $data= array(
               'userName'=>$username,
               'resource'=>$resource,
               'reportType'=>$reportType,
               'dateTimeStamp'=>$dateTime,
               'frequency'=>$frequency,
               'trackerName'=>'testTracker'
        );
        $tracker= $this->ci_obj->tracker_model->insert($data);
        $this->assertEquals($tracker, FALSE, "must fail as the tracker name already exist");
     }
     
     public function test_get_all_trackers()
     {
         $filter= array('userName'=>'unit-test');
         $result= $this->ci_obj->tracker_model->get_all_tracker($filter);
         $this->assertTrue(is_array($result),'Must be the array');
         $this->assertTrue(is_object($result[0]),'Must be object');
         $username = $result[0]->getUsername(); 
         $this->assertEquals($username, 'unit-test','must be the object with property username valued unit-test');  
     }
     
     public function test_get_all_trackers_with_no_filters()
     {
          $result= $this->ci_obj->tracker_model->get_all_tracker();
          $this->assertTrue(is_array($result),'Must be the array');
          $this->assertCount(1, $result,'Must contain 1 element');
     }
     
     public function test_delete_tracker()
     {
        $filter = array('userName' => 'unit-test');
        $this->ci_obj->tracker_model->delete($filter);
        $result= $this->ci_obj->tracker_model->get_all_tracker();
        $this->assertCount(0, $result,'Must contain 0 element after delete');
     }
     
     public function test_update_tracker()
     {
         $filter = array('userName'=>'unit-test', 'trackerName'=>'testTracker');
         $result=$this->ci_obj->tracker_model->update_tracker($filter,array('trackerName'=>'changedTracker'));
         $this->assertEquals($result['updatedExisting'], TRUE);
     }
     
     
}
?>
