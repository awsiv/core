<?php 
 class Bundle extends Cf_Controller
  {
	function Bundle()
	 {
		 parent::__construct();
	 }
	 
	function index()
	{
	$this->blist();
	}
        
	function blist()
	 {
		$hostkey = NULL;
		$name = ".*";
		$regex = 1;
		$hash = NULL;
		$host = NULL;
		$addr = NULL;
		$tago = 0;
		$data=array(
			 'title'=>"Cfengine Mission Portal - Bundles",
             'title_header'=>"search results",
             'nav_text'=>"Services : this host",
      			 'status'=>"current",
      			 'bundle_list'=> cfpr_report_bundlesseen($hostkey,$name,$regex,NULL),
			 ); 
		$this->template->load('template','bundle/bundle_list',$data);
	 }
   
   function details()
   {
     $params=$this->uri->uri_to_assoc(3);
     $bundle=isset($params['bundle'])?$params['bundle']:"";
     $type=isset($params['type'])?$params['type']:"";
     $data=array(
       'title'=>"Cfengine Mission Portal - Bundles",
             'title_header'=>"search results",
             'nav_text'=>"Show : Bundle definition",
             'status'=>"current",
             'bundle'=>$bundle,
             'allbundles'=>cfpr_list_all_bundles(NULL),
             'args'=> cfpr_get_args_for_bundle($bundle,$type),
             'classes' => cfpr_get_classes_for_bundle($bundle,$type),
            'list' => cfpr_list_handles_for_bundle($bundle,$type,False),
            'others'=>cfpr_list_bundles_using($bundle)
            );
    $this->template->load('template','bundle/bundle_detail',$data);
    
   }
 }
