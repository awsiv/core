<?php

class Welcome extends Cf_Controller{

	function __construct()
		{
			parent::__construct();			
			parse_str($_SERVER['QUERY_STRING'],$_GET);
		}
	
	function index()
	{
	    $data=array(
		'title'=>"Cfengine Mission Portal - overview",
                'title_header'=>"overview",
                'nav_text'=>"Home : overview",
		'summary'=>"current"
		);
		$this->template->load('template', 'index',$data);
	}
	
	function status()
	{
	  $data=array(
		'title'=>"Cfengine Mission Portal - engineering status",
        'title_header'=>"engineering status",
        'nav_text'=>"Status : hosts",
		'status'=>"current",
		'ret1' => cfpr_getlicense_owner(),
		'all' => cfpr_count_all_hosts(),
		'r' => cfpr_count_red_hosts(),
		'y' => cfpr_count_yellow_hosts(),
		'g' => cfpr_count_green_hosts(),
		'allreps' => cfpr_select_reports(".*",100),
		'allSppReps' => cfpr_cdp_reportnames()
		);
	  $this->template->load('template', 'status',$data);
	}
	
	function helm()
	{
	  $data=array(
		'title'=>"Cfengine Mission Portal - control",
        'title_header'=>"control",
        'nav_text'=>"Planning : menu",
		'planning'=>"current"
		);
	  $this->template->load('template', 'helm',$data);
	}
	
	function knowledge()
	{
	    $getparams=$this->uri->uri_to_assoc(3);
	    $search = isset($getparams['search'])?$getparams['search']:$this->input->post('search');
            $topic = isset($getparams['topic'])?$getparams['topic']:$this->input->post('topic');
		$pid = isset($getparams['pid'])?$getparams['pid']:$this->input->post('pid');
		$data=array(
		 'search' => $search,
		 'topic' => $topic,
		 'pid'=> $pid,
		 'title'=>"Cfengine Mission Portal - Knowledge bank",
                 'title_header'=>"Knowledge bank",
                 'nav_text'=>"Knowledgebank",
		 'library'=>"current"
		 );
	  $this->template->set('injected_item','<link href="'.get_cssdir().'jquery.fancybox-1.3.1.css" rel="stylesheet" media="screen" />');
	  $this->template->load('template', 'knowledge',$data);
	}
	
	function hosts($type)
	{
	  
	  $data=array(
		 'type'=>$type,
         'title_header'=>"engineering status",
		 'title'=>"Cfengine Mission Portal - ".$type." hosts",
		 'nav_text'=>"Status : hosts",
		 'status'=>"current"
		 );
	  $this->template->load('template', 'hosts',$data);
	}
	
	function host($hostkey=NULL)
	{
	 if(is_null($hostkey))
	 {
	  $hostkey=isset($_POST['hostkey'])? $_POST['hostkey'] : "none";
	 }

	 $hostname=cfpr_hostname($hostkey);
	 $ipaddr=cfpr_ipaddr($hostkey);
	 $username=isset($_POST['username'])? $_POST['username'] : "";
	 $comment_text=isset($_POST['comment_text'])? $_POST['comment_text'] : "";
	 $commentid = cfpr_get_host_commentid($hostkey);
	 $op = isset($_POST['op'])? $_POST['op'] : "";
	
	 if (!is_null($username) && !is_null($comment_text) && $op=="addcomment")
	 {
   	  cfpr_comment_add($hostkey,"",$commentid,1,"$hostname,$ipaddr",$username,time(),$comment_text);
	  $username="";
	  $comment_text="";
	 } 
	 $commentid = cfpr_get_host_commentid($hostkey);

	 $comments=cfpr_comment_query('',$commentid,'',-1,-1);	 

	 $data=array(
	        'hostkey'=>$hostkey,
			'title_header'=>"host ".$hostname,
			'title'=>"Cfengine Mission Portal - host ".$ipaddr,
			'nav_text'=>"Status : host",
			'status'=>"current",
			'hostname'=>$hostname,	
			'ipaddr'=>$ipaddr,			
			'commentid'=>$commentid,
			'op'=>$op,
			'comments'=>$comments
			);
	  $this->template->load('template', 'host',$data); 
	}
	
	function weakest_host()
	{
	$data=array(
			'title_header'=>"weakest hosts",
		    'title'=>"Cfengine Mission Portal - weakest hosts ",
			'nav_text'=>"Status : weakest hosts",
			'status'=>"current",
			'ret' => cfpr_top_n_hosts("compliance",1000),
			);
	 $this->template->load('template', 'topN',$data);
	}
	
	function services()
	{
	$data=array(
			'title_header'=>"service catalogue",
		        'title'=>"Cfengine Mission Portal - service catalogue ",
			'nav_text'=>"Planning : services",
			'planning'=>"current",
			'services' => cfpr_list_all_bundles("agent"),
			);
	$this->template->load('template', 'services',$data);
	}
	
	function license()
	{
	$data=array(
	      'title_header'=>"license usage status",
		  'title'=>"Cfengine Mission Portal - license usage status ",
		  'nav_text'=>"License : status",
		  'planning'=>"current",
	      'ret2' => cfpr_getlicenses_promised(),
		  'ret3' => cfpr_getlicenses_granted(),
		  'expiry' => cfpr_getlicense_expiry(),
		  'txt' => cfpr_getlicense_summary()
		 );
	$this->template->load('template', 'license',$data);
	}
	
	function classes()
	{
	$hostkey = NULL;
	$name = ".*";
	$regex = 1;
	$hash = NULL;
	$host = NULL;
	$addr = NULL;
	$tago = 0;
	$data=array(
	         'title_header'=>"classes",
		 'title'=>"Cfengine Mission Portal - classes ",
		 'nav_text'=>"Status : classes",
		 'status'=>"current",
	         'ret' => cfpr_report_classes($hostkey,$name,$regex,NULL),
	    );
	$this->template->load('template', 'classes',$data);
	}
	
	function cfeditor()
	{
	 redirect('/cfeditor/');
	}
	
	function search()
	{
	 $params = $this->uri->uri_to_assoc(3);
	 redirect('/search/'.$this->uri->assoc_to_uri($params));
	}

        function body()
        {
            $getparams=$this->uri->uri_to_assoc(3);
            $body = isset($getparams['body'])?$getparams['body']:$this->input->post('search');
            $type = isset($getparams['type'])?$getparams['type']:$this->input->post('type');
            $data=array(
	         'title_header'=>"body ".$body,
		 'title'=>"Cfengine Mission Portal - classes ",
		 'nav_text'=>"show : body",
		 'status'=>"current",
	         'allbodies' => cfpr_list_bodies(".*",$type),
                 'def'=> cfpr_get_promise_body($body,$type),
                 'type'=>$type
	    );
	$this->template->load('template', 'body',$data);
            
        }
	
}

/* End of file welcome.php */
/* Location: ./system/application/controllers/welcome.php */