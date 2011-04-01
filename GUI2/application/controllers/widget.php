<?php
class Widget extends CI_Controller
{
     function Widget()
	 {
		 parent::__construct();

	 }


        function hostfinder()
        {
            $this->data['hostlist']=json_decode(cfpr_select_hosts("none", ".*", 100));
            $this->load->view('widgets/hostfinder',$this->data);
        }

        function summaryCompliance($startDate) {

            $this->load->library('cf_table');
            $startDateTimeStamp = $startDate;
            $stopDateTimeStamp = $startDate + (6*3600);
            $this->data['notkept']=json_decode(cfpr_summarize_notkept(NULL,NULL,$startDateTimeStamp,$stopDateTimeStamp,NULL),true);
            $this->data['repaired']=json_decode(cfpr_summarize_repaired(NULL,NULL,$startDateTimeStamp,$stopDateTimeStamp,NULL),true);
            $this->data['startDate']=getDateStatus($startDateTimeStamp,true);
            $this->data['stopDate']=getDateStatus($stopDateTimeStamp,true);
          

            $this->load->view('widgets/summaryCompliance',$this->data);
            
        }

}

?>
