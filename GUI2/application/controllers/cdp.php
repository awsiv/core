<?php
class Cdp extends Cf_Controller
{
  function Cdp()
  {
   parent::__construct();
  }
  
  function index()
  {
	$report_type = isset($_POST['cdp_report'])?$_POST['cdp_report']:"";
	$hostkey = "";
	$data=array(
				 'report_type'=>$report_type,
				 'title'=>"Cfengine Mission Portal - Content-Driven Policy Reports",
				 'title_header'=>"Content-Driven Policy Reports",
				 'nav_text'=>"Status : hosts",
				 'status'=>"current",
				 'report_title'=>$report_type
				);
	
	switch($report_type)
	   {
	   case "ACLs":
		   $data['reportheading'] = "Access control lists (cdp_inputs/acl_directory_list.txt and cdp_inputs/acl_file_list.txt)";
		   break;
		   
	   case "Commands":
		   $data['reportheading'] = "Commands (cdp_inputs/command_list.txt)";
		   break;
		   
	   case "File Changes":
		   $data['reportheading'] = "File changes (cdp_inputs/file_change_list.txt)";
		   break;
		   
	   case "File Diffs":
		   $data['reportheading'] = "File diffs (cdp_inputs/file_diff_list.txt)";
		   break;
		   
	   case "Registry":
		   $data['reportheading'] = "Registry (cdp_inputs/registry_list.txt)";
		   break;
		   
	   case "Services":
		   $data['reportheading'] =  "Services (cdp_inputs/service_list.txt)";
		   break;
		   
	   default:
		   $data['reportheading'] = " Unknown report type ($report_type) ";
		   
	   }
        $data['reporttable'] = cfpr_cdp_report(NULL,$report_type);
		$this->template->load('template','cdp/index',$data);
  }

}

