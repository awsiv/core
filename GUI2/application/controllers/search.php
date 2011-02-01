<?php
class Search extends Controller
{
  function Search()
   {
	 parent::Controller(); 	 
   }
	
   function index()
   {
       $getparams=$this->uri->uri_to_assoc(3);
	   $search = isset($getparams['search'])?$getparams['search']:$this->input->post('search');
       $hostkey = isset($getparams['hostkey'])?$getparams['hostkey']:$this->input->post('hostkey');
       $report_type =isset($getparams['report'])?$getparams['report']:$this->input->post('report');
       $many = isset($getparams['manyhosts'])?$getparams['manyhosts']:$this->input->post('manyhosts');
       $hours_deltafrom = isset($getparams['hours_deltafrom'])?$getparams['hours_deltafrom']:$this->input->post('hours_deltafrom');
       $hours_deltato = isset($getparams['hours_deltato'])?$getparams['hours_deltato']:$this->input->post('hours_deltato');
       $class_regex = isset($getparams['class_regex'])?$getparams['class_regex']:$this->input->post('class_regex');
	   $hosts_only = isset($getparams['hosts_only'])?$getparams['hosts_only']:$this->input->post('hosts_only');
           $state=isset($getparams['state'])?$getparams['state']:$this->input->post('state');
	   
	   $hostname =  cfpr_hostname($hostkey);
	   //necessary for search result view
	   
	   $data=array(
	         'report_type'=>$report_type,
			 'title'=>"Cfengine Mission Portal - search results",
             'title_header'=>"search results",
             'nav_text'=>"Status : $report_type",
			 'status'=>"current",
			 'report_title'=>$report_type
			 );
	   
	   if ($search == "")
		  {
		  $search = ".*";
		  }
		  
		  switch ($report_type)
		  {
		   case "Bundle profile":
		         if($many)
				 {
		         $name = isset($_POST['name'])?$_POST['name']:"";
				 $data['name']=$name;
		          if($hosts_only)// when host only field is checked  to geat a group pf hosts
				     {
					 $data['report_result'] = cfpr_hosts_with_bundlesseen(NULL,$name,true,$class_regex);
					 $this->template->load('template','searchpages/search_result_group',$data);
					 }
				  else
				     {
					 $data['report_result'] = cfpr_report_bundlesseen(NULL,$name,true,$class_regex);
					 $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex);
					 $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/pdfaction/email');
					 $this->template->load('template','searchpages/searchresult',$data);
					 }
				  }
				  elseif($hostkey!="") //when only a particular host is selected
				  {
				     
					 $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class/'.$class_regex);
					 $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class/'.$class_regex.'/pdfaction/email');
					 $data['report_result'] =  cfpr_report_bundlesseen($hostkey,$search,true,$class_regex);
					 $this->template->load('template','searchpages/searchresult',$data);
					 
				  }
				  else //not nothing else is satisfied display extra form for more search paramaters
				  {
				     $this->template->load('template','searchpages/bundleprofile',$data);
				  }
		    break;
			case "Business value report":
			      if($many)
					{
			         $days = isset($_POST['days'])?$_POST['days']:"";
                     $months = isset($_POST['months'])?$_POST['days']:"";
                     $years = isset($_POST['years'])?$_POST['years']:"";
						if($hosts_only)
						{
						 $data['report_title'] = $report_type." Days: $days<br>Months: $months<br>Years: $years";
						 $data['report_result']= cfpr_hosts_with_value(NULL,$days,$months,$years,$class_regex);
						 $this->template->load('template','searchpages/search_result_group',$data);
						}
						else
						{
						 $data['report_title'] = $report_type;
						  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/days/'.$days.'/months/'.$months.'/years/'.$years.'/class_regex/'.$class_regex);
					      $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/days/'.$days.'/months/'.$months.'/years/'.$years.'/class_regex/'.$class_regex.'/pdfaction/email');
						  $data['report_result'] = cfpr_report_value(NULL,$days,$months,$years,$class_regex);
						 $this->template->load('template','searchpages/searchresult',$data);
						}
					}
				  elseif($hostkey!="")
				    {
					 $data['report_title']= $report_type;
					 $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class_regex/'.$class_regex);
                     $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class_regex/'.$class_regex.'/pdfaction/email');
					 $data['report_result'] = cfpr_report_value($hostkey,NULL,NULL,NULL,$class_regex);
					 $this->template->load('template','searchpages/searchresult',$data);
					}
				  else
				    {
					  $this->template->load('template','searchpages/business_value_report',$data);
					}
		    break; 
            case "Class profile":
                  if($many)
				  {
                     $name = isset($_POST['name'])?$_POST['name']:"";
						if($hosts_only)
						{
						 $data['report_result']= cfpr_hosts_with_classes(NULL,$name,true,$class_regex);
						 $this->template->load('template','searchpages/search_result_group',$data);
						}
						else
						{
						  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/class_regex/'.$class_regex);
					      $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/class_regex/'.$class_regex.'/pdfaction/email');
						  $data['report_result'] =  cfpr_report_classes(NULL,$name,true,$class_regex);
						 $this->template->load('template','searchpages/searchresult',$data);
						}
					}
				   elseif($hostkey!="")
				    {
					 $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search);
                     $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/pdfaction/email');
					 $data['report_result'] = cfpr_report_classes($hostkey,$search,true,$class_regex);
					 $this->template->load('template','searchpages/searchresult',$data);
					}
				   else
				    {
					$this->template->load('template','searchpages/class_profile',$data);
					}
			break;
			case "Compliance by promise":
			      if($many)
				  {
				   $name = isset($_POST['name'])?$_POST['name']:"";
				   $state= isset($POST['state'])?$_POST['state']:"";
						if($hosts_only)
						{
						 $data['report_result']= cfpr_hosts_with_compliance_promises(NULL,$name,$state,true,$class_regex);
						 $this->template->load('template','searchpages/search_result_group',$data);
						}
						else
						{
						  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/state/'.$state.'/class_regex/'.$class_regex);
					      $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/state/'.$state.'/class_regex/'.$class_regex.'/pdfaction/email');
						  $data['report_result'] = cfpr_report_compliance_promises(NULL,$name,$state,true,$class_regex);
						 $this->template->load('template','searchpages/searchresult',$data);
						}
				  }
				  elseif($hostkey!="")
				  {
					 $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/state/'.$state.'/class_regex/'.$class_regex);
                     $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/state/'.$state.'/class_regex/'.$class_regex.'/pdfaction/email');
					 $data['report_result'] = cfpr_report_compliance_promises($hostkey,$search,$state,true,$class_regex);
					 $this->template->load('template','searchpages/searchresult',$data); 
				  }
				  else
				  {
				  $this->template->load('template','searchpages/compliance_by_promise',$data);
				  }
			break;
			case "Compliance summary":
			      if($many)
				  {
						if($hosts_only)
						{
						 $data['report_result']= cfpr_hosts_with_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">",$class_regex);
						 $this->template->load('template','searchpages/search_result_group',$data);
						}
						else
						{
						 $data['report_title'] = $report_type;
						  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/class_regex/'.$class_regex.'/hostkey/'.$hostkey);
					      $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/class_regex/'.$class_regex.'/hostkey/'.$hostkey.'/pdfaction/email');
						  $data['report_result'] =  cfpr_report_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">",$class_regex);
						 $this->template->load('template','searchpages/searchresult',$data);
						}
				  }
				  elseif($hostkey!="")
				  {
					 $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class_regex/'.$class_regex);
                     $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class_regex/'.$class_regex.'/pdfaction/email');
					 $data['report_result'] = cfpr_report_compliance_summary($hostkey,NULL,-1,-1,-1,-1,">",$class_regex);
					 $this->template->load('template','searchpages/searchresult',$data); 
				  }
				  else
				  {
				  $this->template->load('template','searchpages/summary_report',$data);
				  }
			break;
			case "File change log":
			     if($many)
				 {
				 $name = isset($_POST['name'])?$_POST['name']:"";
				    if($hosts_only)
						{
						 $data['report_result']=  cfpr_hosts_with_filechanges(NULL,$name,true,-1,">",$class_regex);
						 $this->template->load('template','searchpages/search_result_group',$data);
						}
						else
						{
						  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/class_regex/'.$class_regex.'/hostkey/'.$hostkey);
					      $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/class_regex/'.$class_regex.'/hostkey/'.$hostkey.'/pdfaction/email');
						  $data['report_result'] =  cfpr_report_filechanges(NULL,$name,true,-1,">",$class_regex);
						  $this->template->load('template','searchpages/searchresult',$data);
						}
				 }
				 elseif($hostkey!="")
				 {
				 $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class_regex/'.$class_regex);
				 $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class_regex/'.$class_regex.'/pdfaction/email');
			     $data['report_result'] =  cfpr_report_filechanges($hostkey,$search,true,-1,">",$class_regex); 
				 $this->template->load('template','searchpages/searchresult',$data);
				 }
				 else
				 {
				 $this->template->load('template','searchpages/file_change_log',$data);
				 }
			break;
			case "File change diffs":
			     $cal=-1;
			     if($many)
				 {
				  $name = isset($_POST['name'])?$_POST['name']:"";
				  $diff= isset($POST['diff'])?$_POST['name']:"";
						if($hosts_only)
						{
						 $data['report_result']= cfpr_hosts_with_filediffs(NULL,$name,$diff,true,$cal,">",$class_regex);
						 $this->template->load('template','searchpages/search_result_group',$data);
						}
						else
						{
						  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/diff/'.$diff.'/cal/'.$cal.'/class_regex/'.$class_regex);
					      $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/diff/'.$diff.'/cal/'.$cal.'/class_regex/'.$class_regex.'/pdfaction/email');
						  $data['report_result'] =  cfpr_report_filediffs(NULL,$name,$diff,true,$cal,">",$class_regex);
						 $this->template->load('template','searchpages/searchresult',$data);
						}
				 }
				 elseif($hostkey!="")
				 {
				    $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/cal/'.$cal.'/class_regex/'.$class_regex);
                     $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/cal/'.$cal.'/class_regex/'.$class_regex.'/pdfaction/email');
					 $data['report_result'] = cfpr_report_filediffs($hostkey,$search,NULL,true,$cal,">",$class_regex);
					 $this->template->load('template','searchpages/searchresult',$data);
				 }
				 else
				 {
				   $this->template->load('template','searchpages/file_change_diffs',$data);
				 }
            break;
            case "Last saw hosts":
                if($many)
                {
				$name = isset($_POST['name'])?$_POST['name']:"";
				$address = isset($_POST['address'])?$_POST['address']:"";
				$key = isset($_POST['key'])?$_POST['key']:"";
				$ago = isset($_POST['ago'])?$_POST['ago']:"";
				$ago= $ago==0?-1:$ago;
				if($hosts_only)// when host only field is checked  to geat a group pf hosts
				     {
					 $data['report_result'] = cfpr_hosts_with_lastseen(NULL,$key,$name,$address,$ago,true,$class_regex);
					 $this->template->load('template','searchpages/search_result_group',$data);
					 }
				  else
				     {
					 $data['report_result'] = cfpr_report_lastseen(NULL,$key,$name,$address,$ago,true,$class_regex);
					 $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/key/'.$key.'/address/'.$address.'/ago'.$ago);
					 $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/key/'.$key.'/address/'.$address.'/ago'.$ago.'/pdfaction/email');
					 $this->template->load('template','searchpages/searchresult',$data);
					 }
                }
                elseif($hostkey!="")
                {
				     $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class/'.$class_regex.'/ago/-1');
					 $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class/'.$class_regex.'/pdfaction/email/ago/-1');
					 $data['report_result'] =  cfpr_report_lastseen($hostkey,NULL,$search,NULL,-1,true,$class_regex);
					 $this->template->load('template','searchpages/searchresult',$data);
				
                }
                else
                {
				   $this->template->load('template','searchpages/last_saw_hosts',$data);
                }	
        break;
        case "Patches available":
              if ($many)
                {
				 $name = isset($_POST['name'])?$_POST['name']:"";
                 $version = isset($_POST['version'])?$_POST['version']:"";
                 $arch = isset($_POST['arch'])?$_POST['arch']:"";
					 if($hosts_only)
					 {
					  $data['report_result'] = cfpr_hosts_with_patch_avail(NULL,$name,$version,$arch,true,$class_regex);
					  $this->template->load('template','searchpages/search_result_group',$data);
					 }
					 else
					 {
					 $data['report_result'] = cfpr_report_patch_avail(NULL,$name,$version,$arch,true,$class_regex);
					 $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/version/'.$version.'/arch/'.$arch);
					 $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/version/'.$version.'/arch/'.$arch.'/pdfaction/email');
					 $this->template->load('template','searchpages/searchresult',$data);
					 }
                }
              elseif($hostkey!="")
                {
				$data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class/'.$class_regex);
			    $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class/'.$class_regex.'/pdfaction/email');
			    $data['report_result'] =  cfpr_report_patch_avail($hostkey,$search,NULL,NULL,true,$class_regex);
				$this->template->load('template','searchpages/searchresult',$data);
                }
              else
                {
				 $this->template->load('template','searchpages/patches_available',$data);
                }
        break;
        case "Patch status":
             if($many)
              {
			  $name = isset($_POST['name'])?$_POST['name']:"";
              $version = isset($_POST['version'])?$_POST['version']:"";
              $arch = isset($_POST['arch'])?$_POST['arch']:"";
				  if($hosts_only)
				   {
				   $data['report_result'] = cfpr_hosts_with_patch_in(NULL,$name,$version,$arch,true,$class_regex);
				   $this->template->load('template','searchpages/search_result_group',$data);
				   }
				  else
				   {
				   $data['report_result'] = cfpr_report_patch_in(NULL,$name,$version,$arch,true,$class_regex);
				   $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/version/'.$version.'/arch/'.$arch);
				   $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/version/'.$version.'/arch/'.$arch.'/pdfaction/email');
				   $this->template->load('template','searchpages/searchresult',$data);
				   }
              }
             elseif($hostkey!="")
              {
			  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class/'.$class_regex);
			  $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class/'.$class_regex.'/pdfaction/email');
			  $data['report_result'] =  cfpr_report_patch_in($hostkey,$search,NULL,NULL,true,$class_regex);
			  $this->template->load('template','searchpages/searchresult',$data);
              }
             else
              {
			  $this->template->load('template','searchpages/patch_status',$data);
              }
        break;
		case "Performance":
		    if($many)
			  {
			  $name = isset($_POST['name'])?$_POST['name']:"";
			  if($hosts_only)
				   {
				   $data['report_result'] =  cfpr_hosts_with_performance(NULL,$name,true,$class_regex);
				   $this->template->load('template','searchpages/search_result_group',$data);
				   }
				  else
				   {
				   $data['report_result'] = cfpr_report_performance(NULL,$name,true,$class_regex);
				   $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex);
				   $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/pdfaction/email');
				   $this->template->load('template','searchpages/searchresult',$data);
				   }
			  }
			elseif($hostkey!="")
			  {
			  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class/'.$class_regex);
			  $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/search/'.$search.'/class/'.$class_regex.'/pdfaction/email');
			  $data['report_result'] =  cfpr_report_performance($hostkey,$search,true,$class_regex);
			  $this->template->load('template','searchpages/searchresult',$data);
			  }
			else
			  {
			  $this->template->load('template','searchpages/performance',$data);
			  }
        break;
        case "Promises repaired log":
        case "Promises repaired summary":
            if($many)
              {
			  $name = isset($_POST['name'])?$_POST['name']:"";
			  if($hosts_only)
				   {
				   $data['report_result'] =  cfpr_hosts_with_repaired(NULL,$name,intval($hours_deltafrom),intval($hours_deltato),$class_regex);
				   $this->template->load('template','searchpages/search_result_group',$data);
				   }
				  else
				   {
				   $data['report_result'] = cfpr_report_repaired(NULL,$name,intval($hours_deltafrom),intval($hours_deltato),$class_regex);
				   $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/hours_deltafrom/'.$hours_deltafrom.'/hours_deltato/'.$hours_deltato);
				   $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/pdfaction/email'.$hours_deltafrom.'/hours_deltato/'.$hours_deltato);
				   $this->template->load('template','searchpages/searchresult',$data);
				   }
              }
            elseif($hostkey!="")
              {
			  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class/'.$class_regex);
			  $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class/'.$class_regex.'/pdfaction/email');
			   if($report_type=="Promises repaired log")
			      $data['report_result'] =  cfpr_report_repaired($hostkey,NULL,0,0,$class_regex);
			   if($report_type=="Promises repaired summary")
			      $data['report_result'] = cfpr_summarize_repaired($hostkey,NULL,NULL);
			  $this->template->load('template','searchpages/searchresult',$data);
              }
            else
              {
			  $this->template->load('template','searchpages/promises_repaired_log',$data);
              }
	    break;
        case "Promises not kept summary":
        case "Promises not kept log":
		     if($many)
			  {
			$name = isset($_POST['name'])?$_POST['name']:"";
			   if($hosts_only)
				   {
				   $data['report_result'] =  cfpr_hosts_with_notkept(NULL,$name,intval($hours_deltafrom),intval($hours_deltato),$class_regex);
				   $this->template->load('template','searchpages/search_result_group',$data);
				   }
				  else
				   {
				   $data['report_result'] =  cfpr_report_notkept(NULL,$name,intval($hours_deltafrom),intval($hours_deltato),$class_regex);
				   $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/hours_deltafrom/'.$hours_deltafrom.'/hours_deltato/'.$hours_deltato);
				   $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/pdfaction/email'.$hours_deltafrom.'/hours_deltato/'.$hours_deltato);
				   $this->template->load('template','searchpages/searchresult',$data);
				   }
			  }
			 elseif($hostkey!="")
			  {
			  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class/'.$class_regex);
			  $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class/'.$class_regex.'/pdfaction/email');
			   if($report_type=="Promises not kept summary")
			      $data['report_result'] =   cfpr_summarize_notkept($hostkey,NULL,NULL);
			   if($report_type=="Promises not kept log")
			      $data['report_result'] = cfpr_report_notkept($hostkey,NULL,0,0,$class_regex);
			  $this->template->load('template','searchpages/searchresult',$data);
			  }
			 else
			 {
			 $this->template->load('template','searchpages/promises_not_kept',$data);
			 }
        break; 
        case "Setuid/gid root programs":
		    if($many)
			 {
			 $name = isset($_POST['name'])?$_POST['name']:"";
			   if($hosts_only)
				   {
				   $data['report_result'] = cfpr_hosts_with_setuid(NULL,$name,true,$class_regex);
				   $this->template->load('template','searchpages/search_result_group',$data);
				   }
				  else
				   {
				   $data['report_result'] =  cfpr_report_setuid(NULL,$name,true,$class_regex);
				   $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex);
				   $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/pdfaction/email');
				   $this->template->load('template','searchpages/searchresult',$data);
				   }
			 }
			 elseif($hostkey!="")
			 {
			  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class/'.$class_regex.'/search/'.$search);
			  $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class/'.$class_regex.'/search/'.$search.'/pdfaction/email');
			  $data['report_result'] = cfpr_report_setuid($hostkey,$search,true,$class_regex);
			  $this->template->load('template','searchpages/searchresult',$data);
			 }
			 else
			 {
			 $this->template->load('template','searchpages/uid_gid_root_programs',$data);
			 }
        break;
        case "Software installed":
		    if($many)
			{
			 $name = isset($_POST['name'])?$_POST['name']:"";
             $version = isset($_POST['version'])?$_POST['version']:"";
             $arch = isset($_POST['arch'])?$_POST['arch']:"";
			   if($hosts_only)
				   {
				   $data['report_result'] = cfpr_hosts_with_software_in(NULL,$name,$version,$arch,true,$class_regex);
				   $this->template->load('template','searchpages/search_result_group',$data);
				   }
				  else
				   {
				   $data['report_result'] =  cfpr_report_software_in(NULL,$name,$version,$arch,true,$class_regex);
				   $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/version/'.$version.'/arch/'.$arch);
				   $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/search/'.$name.'/class_regex/'.$class_regex.'/version/'.$version.'/arch/'.$arch.'/pdfaction/email');
				   $this->template->load('template','searchpages/searchresult',$data);
				   }
			}
			elseif($hostkey!="")
			{
			  $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class/'.$class_regex.'/search/'.$search);
			  $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class/'.$class_regex.'/search/'.$search.'/pdfaction/email');
			  $data['report_result'] = cfpr_report_software_in($hostkey,$search,NULL,NULL,true,$class_regex);
			  $this->template->load('template','searchpages/searchresult',$data);
			}
			else
			{
			$this->template->load('template','searchpages/software_installed',$data);
			}
        break;
        case "Variables":
            if($many)
            {
			$scope = isset($_POST['scope'])?$_POST['scope']:"";
            $lval = isset($_POST['lval'])?$_POST['lval']:"";
            $rval = isset($_POST['rval'])?$_POST['rval']:"";
            $type = isset($_POST['type'])?$_POST['type']:"";
			if($hosts_only)
				   {
				   $data['report_result'] = cfpr_hosts_with_vars(NULL,$scope,$lval,$rval,$type,true,$class_regex);
				   $this->template->load('template','searchpages/search_result_group',$data);
				   }
				  else
				   {
				   $data['report_result'] =  cfpr_report_vars(NULL,$scope,$lval,$rval,$type,true,$class_regex);
				   $data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/class_regex/'.$class_regex.'/scope/'.$scope.'/lval/'.$lval.'/rval/'.$rval.'/var_type/'.$type);
				   $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/class_regex/'.$class_regex.'/scope/'.$scope.'/lval/'.$lval.'/rval/'.$rval.'/var_type/'.$type.'/pdfaction/email');
				   $this->template->load('template','searchpages/searchresult',$data);
				   }
            }
            elseif($hostkey!="")
            {
			$data['report_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class/'.$class_regex.'/search/'.$search);
			  $data['email_link']=site_url('/pdfreports/index/type/'.$report_type.'/hostkey/'.$hostkey.'/class/'.$class_regex.'/search/'.$search.'/pdfaction/email');
			  $data['report_result'] =cfpr_report_vars($hostkey,NULL,$search,NULL,NULL,true,$class_regex);
			  $this->template->load('template','searchpages/searchresult',$data);
            }
            else
            {
			 $this->template->load('template','searchpages/variables',$data);
            }
        break;
        default:
             $this->template->load('template','searchpages/nodata',$data);		
	    }
	       
   }
   
}
