<?php
class test_reports_libraries extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {

    }

    public function tearDown() {

    }


    function __testcases($data)
    {
         $array =json_decode(utf8_encode($data),true);
         $retValue = json_last_error();
         $this->assertTrue(is_array($array), "Should Return a valid array");
         $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        if(isset($array['data']))
        {
           $this->assertTrue(!empty ($array),"Is not empty ". count($array['data']) ."items");
        }
        else
        {
           $this->assertTrue(!empty ($array),"Is not empty ". count($array) ."items");
        }
        
    }
    public function test_cfpr_hosts_with_bundlesseen(){
        $data=cfpr_hosts_with_bundlesseen(NULL, NULL, true, false);
      $this->__testcases($data);
    }
    
    public function test_cfpr_report_bundlesseen(){
        $data=cfpr_report_bundlesseen(NULL, NULL, true, false, NULL, NULL);
        $this->__testcases($data);
    }
    //busineess value report
    
   /* public function test_cfpr_hosts_with_value(){
        $data=cfpr_hosts_with_value(NULL, NULL, NULL, NULL, false);
        $array =json_decode(utf8_encode($data),true);
         $retValue = json_last_error();
        $this->assertTrue(is_array($array), "Should Return a valid array");
         $this->dump($data);
        $this->assertFalse($retValue, "This should return 0 in case of no error, returned value is $retValue");
        $this->assertTrue(!empty ($array),"Is not empty ". count($array['data']) ."items");
    }*/
    
    public function  test_cfpr_report_value(){
        $data=cfpr_report_value(NULL, NULL, NULL, NULL, false, NULL, NULL);
        $this->__testcases($data);
    }

    public function test_cfpr_hosts_with_classes(){
        $data=cfpr_hosts_with_classes(NULL, NULL, true, false);
         $this->__testcases($data);
    }

    public function test_cfpr_report_classes(){
        $data=cfpr_report_classes(NULL, NULL, true, false, NULL, NULL);
        $this->__testcases($data);
    }


    
 /**
  * complience promise test
  */
    public function test_cfpr_hosts_with_compliance_promises(){
       $data= cfpr_hosts_with_compliance_promises(NULL,NULL, NULL, true, false);
       $this->__testcases($data);
    }

    public function test_cfpr_report_compliance_promises(){
        $data=cfpr_report_compliance_promises(NULL, NULL, NULL, true, false, NULL, NULL);
        $this->__testcases($data);
    }



    /**
     * Compliance Summary test
     */
    public function test_cfpr_hosts_with_compliance_summary(){
        $data=cfpr_hosts_with_compliance_summary(NULL, NULL, -1, -1, -1, -1, ">", false);
        $this->__testcases($data);
    }

    public function test_cfpr_report_compliance_summary()
    {
        $data=cfpr_report_compliance_summary(NULL, NULL, -1, -1, -1, -1, ">", false, NULL, NULL);
        $this->__testcases($data);
    }

    /**
     * File change log
     */
    public function test_cfpr_hosts_with_filechanges(){
        $data=cfpr_hosts_with_filechanges(NULL, NULL, true, -1, ">", false);
        $this->__testcases($data);
    }

    public function test_cfpr_report_filechanges(){
        $data=cfpr_report_filechanges(NULL, NULL, true, -1, ">", false, NULL, NULL);
       $this->__testcases($data);
    }

    public function test_cfpr_report_filechanges_longterm(){
        $data=cfpr_report_filechanges_longterm(NULL, NULL, true, -1, ">", false, NULL, NULL);
         $this->__testcases($data);
    }
    
    
    /**
     *
     * File change diff
     */
    public function test_cfpr_hosts_with_filediffs(){
        $data=cfpr_hosts_with_filediffs(NULL, NULL, NULL, true, NULL, ">", false);
        $this->__testcases($data);
    }

    public function test_cfpr_report_filediffs(){
        $data=cfpr_report_filediffs(NULL, NULL, NULL, true, NULL, ">", false, NULL, NULL);
        $this->__testcases($data);
    }

    public function test_cfpr_report_filediffs_longterm()
    {
        $data=cfpr_report_filediffs_longterm(NULL, NULL, NULL, true, NULL, ">", false, NULL, NULL);
        $this->__testcases($data);
    }

    /**
     * Laste seen Hosts
     */
    public function test_cfpr_hosts_with_lastseen(){
        $data=cfpr_hosts_with_lastseen(NULL,NULL, NULL, NULL, NULL, true, false);
        $this->__testcases($data);
    }

    public function test_cfpr_report_lastseen(){
        $data=cfpr_report_lastseen(NULL, NULL, NULL,NULL, NULL, true, false, NULL, NULL);
        $this->__testcases($data);
    }
    
    /**
     * patches available
     */

    public function test_cfpr_hosts_with_patch_avail(){
        $data=cfpr_hosts_with_patch_avail(NULL, NULL, NULL, NULL, true, false);
        $this->__testcases($data);
    }

    public function test_cfpr_report_patch_avail(){
        $data=cfpr_report_patch_avail(NULL,NULL, NULL, NULL, true, false, NULL, NULL);
        $this->__testcases($data);
    }

    /**
     * patch status
     */
    public function test_cfpr_hosts_with_patch_in(){
        $data=cfpr_hosts_with_patch_in(NULL,NULL,NULL,NULL, true, false);
        $this->__testcases($data);
    }

    public function test_cfpr_report_patch_in(){
        $data=cfpr_report_patch_in(NULL, NULL, NULL, NULL, true, false, NULL, NULL);
        $this->__testcases($data);
    }

    /**
     * Performance
     */

    public function test_cfpr_hosts_with_performance()
    {
      $data=cfpr_hosts_with_performance(NULL, NULL, true, false);
      $this->__testcases($data);
    }

    public function test_cfpr_report_performance(){
        $data=cfpr_report_performance(NULL, NULL, true, false, NULL, NULL);
        $this->__testcases($data);
    }

    /**
     * Repaired
     */
    public function test_cfpr_hosts_with_repaired(){
        $data=cfpr_hosts_with_repaired(NULL, NULL, NULL, NULL, false);
        $this->__testcases($data);
    }

    public function test_cfpr_report_repaired(){
        $data=cfpr_report_repaired(NULL, NULL, 0, 0,false,NULL, NULL);
        $this->__testcases($data);
    }

    public function test_cfpr_summarize_repaired(){
        $data=cfpr_summarize_repaired(NULL, NULL, NULL, NULL, NULL);
         $this->__testcases($data);
    }

    /**
     * promise not kept
     */

    public function test_cfpr_hosts_with_notkept(){
        $data=cfpr_hosts_with_notkept(NULL, NULL, NULL, NULL, false);
        $this->__testcases($data);
    }

    public function test_cfpr_summarize_notkept(){
        $data=cfpr_summarize_notkept(NULL, NULL, NULL, NULL, NULL);
        $this->__testcases($data);
    }

    public function test_cfpr_report_notkept(){
        $data=cfpr_report_notkept(NULL, NULL, NULL, NULL, false, NULL, NULL);
         $this->__testcases($data);
    }

    /**
     * setuid guid
     */
    public function test_cfpr_hosts_with_setuid(){
        $data=cfpr_hosts_with_setuid(NULL, NULL, true, false);
        $this->__testcases($data);
    }

    public function test_cfpr_report_setuid(){
        $data= cfpr_report_setuid(NULL, NULL, true, false,NULL, NULL);
        $this->__testcases($data);
    }

    /**
     * Software installed
     */

     public function test_cfpr_hosts_with_software_in(){
         $data= cfpr_hosts_with_software_in(NULL, NULL, NULL, NULL, true,false);
         $this->__testcases($data);
     }

     public function test_cfpr_report_software_in(){
         $data=cfpr_report_software_in(NULL,NULL, NULL, NULL, true, false, NULL, NULL);
         $this->__testcases($data);
     }

     /**
      * Variables
      */

     public function test_cfpr_hosts_with_vars(){
         $data=cfpr_hosts_with_vars(NULL, NULL, NULL, NULL, NULL, true, false);
         $this->__testcases($data);
     }

     public function test_cfpr_report_vars(){
         $data= cfpr_report_vars(NULL, NULL, NULL, NULL, NULL, true, false, NULL, NULL);
         $this->__testcases($data);
     }
}
?>
