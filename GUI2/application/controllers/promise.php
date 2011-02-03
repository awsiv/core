<?php 
class Promise extends Controller
 {
	function Promise()
	{
		parent::Controller();
	}
	
	function plist()
	{
		$hostkey = NULL;
		$name = ".*";
		$regex = 1;
                $handle = NULL;
		$data=array(
			 'title' => "Cfengine Mission Portal - compliance",
                         'title_header'=>"Compliance",
                         'nav_text'=>"Status : Promises",
      		         'status'=>"current",
      		         'promise_list'=> cfpr_report_compliance_promises($hostkey,$handle,"x",false,NULL),
			 ); 
		$this->template->load('template','promise/promises',$data);
	}
	
	function details($handle=NULL)
	{
		if(is_null($handle))
		{
		 $handle = isset($_POST['handle'])? $_POST['handle']:NULL;
		}
                $mybundle = cfpr_get_promise_bundle($handle);
		$promiser = cfpr_get_promiser($handle);
                $type = cfpr_get_promise_type($handle);
		 $data=array(
				'handle'=>$handle,
				'title_header'=>"promise $handle",
				'title'=>"Cfengine Mission Portal - promise $handle",
				'nav_text'=>"Show : Promise",
				'status'=>"current",
				'pid' => cfpr_get_pid_for_topic("promises",$handle),
				'mybundle' => cfpr_get_promise_bundle($handle),
				'allhandles' => cfpr_list_handles_for_bundle($mybundle,"agent",false),
				'allhandlespromiser'=>cfpr_list_handles($promiser,"",false),
				'type'=>$type,
				'allhandlesbytype' => cfpr_list_handles("",$type,false),
				'promise' => cfpr_summarize_promise($handle)
				);
		 $this->template->load('template', 'promise/promise',$data);
	}
 }
