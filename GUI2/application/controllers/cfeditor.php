<?php
 class Cfeditor extends Cf_Controller {
  
  function Cfeditor()
   {
   parent::__construct();
   $this->load->library('jcryption');
   $this->load->model('repository_model');
    $this->carabiner->js('jquery.tablesorter.min.js');
    $this->carabiner->js('picnet.jquery.tablefilter.js');
    $this->carabiner->js('jquery.tablesorter.pager.js');
   //session_start();
   }
  
   function index()
   {
    $working_dir = get_policiesdir().$this->session->userdata('username');
     $params=array(
			'workingdir' => $working_dir
			);
     $this->load->model('repository_model');
     $this->load->library('cfsvn',$params);
     try{
       $rev=$this->cfsvn->get_working_revision();
       $current_repo=$this->cfsvn->get_current_repository();
       $total_approvals=$this->repository_model->get_total_approval_count($current_repo);
        $data=array(
         'title'=>"Cfengine Mission Portal - Policy editor",
         'revision'=>$rev,
         'total_approvals'=>$total_approvals,
         'curreny_repo'=>$current_repo
		 );
     }catch(Exception $e)
     {
        $data=array(
         'title'=>"Cfengine Mission Portal - Policy editor",
         'revision'=>'Unknown',
         'total_approvals'=>"Unknown",
         'curreny_repo'=>'Empty Working Directory'
	 );
     }
     
   $this->carabiner->css('cfeditor.css');
   $this->carabiner->css('jquery.jnotify.css');
    $this->carabiner->js('jqueryFileTree.js');
    $this->carabiner->js('jquery.jnotify.min.js');
    $this->carabiner->js('widgets/notes.js');
    
   $this->load->view('cfeditor/Cfeditor',$data);
   }
   
   
   //controller function to see if the session have been previously checked out or not
   function is_checked_out()
   {
    if(file_exists(get_policiesdir().$this->session->userdata('username').'/') ) {
			$files = scandir(get_policiesdir().$this->session->userdata('username').'/');
			natcasesort($files);
		    if( count($files) > 2 ) {
			 $checked = true;	
			}
		}
		echo $checked;
	return;
   }
   
   function get_list()
   {
      $path=$this->input->post('dir')?$this->input->post('dir'): get_policiesdir().$this->session->userdata('username').'/';
      $inconttext=basename($path);
      echo "<ul class=\"jqueryFileTree\">";
	    if(file_exists( $path) ) {
	    $files = scandir( $path);
	    natcasesort($files);
		if( count($files) > 2 ) {
                 // All dirs
		foreach( $files as $file ) {
			if( file_exists( $path . $file) && $file != '.' && $file != '..'&&$file != '.svn' && is_dir( $path. $file) ) {
				echo "<li id=\"$inconttext\" class=\"directory collapsed\"><a href=\"#\" rel=\"" . htmlentities( $path.$file) . "/\">" . htmlentities($file) . "</a></li>";
			}
		}
		// All files
			// All files
			$i=1;
			foreach( $files as $file ) {
				if( file_exists( $path.$file) && $file != '.' && $file != '..' && !is_dir( $path.$file) ) {
					$ext = preg_replace('/^.*\./', '', $file);
					echo "<li class=\"file ext_$ext\"><a href=\"#\" rel=\"".htmlentities( $path.$file) . "\" id=\"$inconttext"."_policy_".$i."\">" . htmlentities($file) .  "</a></li>";
					$i++;
				}
			}
		}
      }
	echo "</ul>";
   }
   
   //function for clearing the files in working directory as effect of double checkout
   function clear_dir()
   {
	   $this->load->helper('directory');
	   $ret=deleteAll(get_policiesdir().$this->session->userdata('username').'/',true);
	   echo $ret;
   }
   
   function get_keys()
   {
    $keyLength = 256;
		$keys = $this->jcryption->generateKeypair($keyLength);
		$_SESSION["e"] = array("int" => $keys["e"], "hex" => $this->jcryption->dec2string($keys["e"],16));
		$_SESSION["d"] = array("int" => $keys["d"], "hex" => $this->jcryption->dec2string($keys["d"],16));
		$_SESSION["n"] = array("int" => $keys["n"], "hex" => $this->jcryption->dec2string($keys["n"],16));

		echo '{"e":"'.$_SESSION["e"]["hex"].'","n":"'.$_SESSION["n"]["hex"].'","maxdigits":"'.intval($keyLength*2/16+3).'"}';
   }
   
    //controller function to handle the check out
   function checkout()
   {
   $password=$this->jcryption->decrypt($this->input->post('passwd'),$_SESSION["d"]["int"],$_SESSION["n"]["int"]);
   $params=array(
	                 'username' =>  $this->input->post('user'),
			'password' => $password,
			'repository' => $this->input->post('repo'),
			'workingdir' => get_policiesdir().$this->session->userdata('username')
			);
    $this->load->library('cfsvn',$params);
    $data=array();
    try{
         $data=$this->cfsvn->cfsvn_checkout();
       //if check out was sucessfull
        if($data['status'])
        {
         $data['total_approvals']=$this->repository_model->get_total_approval_count($this->input->post('repo'));
         $this->repository_model->insert_svn_log($this->session->userdata('username'),$this->input->post('repo'), $data['rev'] ,'checkout');
        }
    }
    catch(Exception $e) {
        $data['status']=false;
        $data['message']=$e->getMessage();
    }
    
    echo json_encode($data);	
   }
   
   function commit()
   {
     $data="";
     $currentUser = $this->session->userdata('username');
     $working_dir=get_policiesdir().$this->session->userdata('username');
     $this->load->library('cfsvn', array('workingdir' =>$working_dir));

      try{
     $current_repo=$this->cfsvn->get_current_repository();
     $obj = $this->repository_model->get_specific_repository($currentUser, $current_repo);
     if ($obj != NULL) {
            $info = array('userId' => $obj->userId, 'password' => $obj->password);
            $username = $obj->username;
            $password = $this->repository_model->decrypt_password($info);
            if(!$this->input->post('file'))
		 {
                 $working_dir=$working_dir.'/'.$this->input->post('file');
		 }
             $params=array(
	                'username' => $username,
			'password' => $password,
			'workingdir' => $working_dir
			);
                $this->cfsvn->addcredentials($params);
                $cdetails=$this->cfsvn->cfsvn_commit($this->input->post('comments'));
                //on sucessfull commit of files make a record in data base svnlogs [revision,date,username] in cdetails
                 if(is_array($cdetails) && $cdetails[0] > 0 )
                 {
                    $this->repository_model->insert_svn_log($this->session->userdata('username'),$current_repo, $cdetails[0] ,'commit');
                    $data=array('status'=>true,'rev'=>$cdetails[0]);
                 }
                else
                {
                   $data=array('status'=>false,'message'=>"Not Committed as no changes detected");
                }
          }
     }catch(Exception $e){
                 $data=array('status'=>false,'message'=>$e->getMessage());
             }
     //$password=$this->jcryption->decrypt($this->input->post('passwd'),$_SESSION["d"]["int"],$_SESSION["n"]["int"]);
    // echo json_encode ($cdetails);
             if($data=="")
             {
                  $data=array('status'=>false,'message'=>$this->input->post('file')." Was not comitted");
             }
     echo json_encode ($data);
   }
   
   function update()
   {
     $data="";
     $currentUser = $this->session->userdata('username');
     $this->load->library('cfsvn', array('workingdir' => get_policiesdir().$this->session->userdata('username')));
     try{
     $current_repo=$this->cfsvn->get_current_repository();
     $obj = $this->repository_model->get_specific_repository($currentUser, $current_repo);
      if ($obj != NULL) {

            $info = array('userId' => $obj->userId, 'password' => $obj->password);
            $username = $obj->username;
            $password = $this->repository_model->decrypt_password($info);
             $params=array(
	                'username' => $username,
			'password' =>  $password,
			);
             $this->cfsvn->addcredentials($params);
         
                //gets the revision number
             $cdetails=$this->cfsvn->cfsvn_update();
             $current_repo=$this->cfsvn->get_current_repository();
             $total_approvals=$this->repository_model->get_total_approval_count($current_repo);
             //make a entry in svn log records in our db
             if($cdetails)
                 {
                   $this->repository_model->insert_svn_log($this->session->userdata('username'),$current_repo, $cdetails ,'update');
                 }
               $data=array('status'=>true,'rev'=>$cdetails,'total_approvals'=>$total_approvals);
             }
        }
        catch(Exception $e)
             {
               $data=array('status'=>false,'message'=>$e->getMessage());
             }
     echo json_encode($data);
   }
   
   function svnlogs()
   {
   $no_of_records=50;
   //$password=$this->jcryption->decrypt($this->input->post('passwd'),$_SESSION["d"]["int"],$_SESSION["n"]["int"]);
    $data="";
    $currentUser = $this->session->userdata('username');
    $this->load->library('cfsvn', array('workingdir' => get_policiesdir().$this->session->userdata('username')));

    try{
        $current_repo=$this->cfsvn->get_current_repository();
        $obj = $this->repository_model->get_specific_repository($currentUser, $current_repo);
        if ($obj != NULL) {
            $info = array('userId' => $obj->userId, 'password' => $obj->password);
            $username = $obj->username;
            $password = $this->repository_model->decrypt_password($info);
            $params=array(
	                'username' => $username,
			'password' => $password,
                        'repository'=> $current_repo
			);    
           $this->cfsvn->addcredentials($params);
           $data=array('logs'=>$this->cfsvn->cfsvn_log($no_of_records));
            $this->load->view('cfeditor/svnlogs',$data);
      }
    }catch(Exception $e)
    {
        echo $e->getMessage();
    }
     
	
	/*$logs=array(
			'status'=>'success',
			'table'=>$this->load->view('cfeditor/svnlogs',$data,true)
			 );*/
	//echo json_encode ($logs);
   }
   
   function get_contents()
   {
        $file_path=$this->input->post('file_path');
		$filename= basename($file_path);
		$contents = file_get_contents (realpath($file_path));
		$details=array(
		'filename'=>$filename,
		'content'=>$contents
	     );
	    echo json_encode($details);
   
   }
   
   function compare_contents()
   {
   	    $status="unchanged";
  		//$file_path=get_policiesdir().$this->session->userdata('username').'/'.$this->input->post('file');
               $file_path=$this->input->post('file');
		$contents = file_get_contents($file_path);

                $newcontents=str_replace('\\\\', '\\' , $_POST['newcontents']);
//                $newcontents=str_replace('\\"', '"' , $newcontents);
                $newcontent=str_replace('&gt;', '>' , $newcontents);

		if($this->input->post('agent')=='webkit')
		{
		$newcontents=substr($newcontents, 0, -3);
		}
		if(strcmp($contents,$newcontents))
		{
		$status="changed";	
		}
		$data=array(
		'status'=>$status,
		'old'=>$contents,
		'newd'=>$newcontents,
	     );
		echo json_encode($data);
   
   }
 
   function save_contents()
   {
		$working_dir = get_policiesdir().$this->session->userdata('username');
		if(!file_exists($working_dir))
		  {
			mkdir($working_dir,0700);
		  }
		//$filetobesaved=$working_dir.'/'.$this->input->post('file');
                  $filetobesaved=$this->input->post('file');
                 if($this->input->post('filestats')=='new')
                     {
                    $filetobesaved=$working_dir.'/'.$this->input->post('file');
                     }
		$msg="";
		$written=false;
		if(file_exists($filetobesaved)&&$this->input->post('filestats')=='old')
			{
				//open file for writng and place pointer at the end
                              try{
                                   $this->load->helper('directory');
                                   $written= write_savefile($filetobesaved, $this->input->post('content'));
                                    $msg= "success writing to file";
                                 }
                                 catch (Exception $e){
                                    $written=false;
                                   $msg= "Error writing to file";
                               }
			}
			elseif(file_exists($filetobesaved)&&$this->input->post('filestats')=='new')
			{
				$msg= "couldn't write file <i>$filetobesaved</i>,File with same name exist in repository";
			}
			else
			{
			   try{
                               $this->load->helper('directory');
                               $written= write_savefile($filetobesaved, $this->input->post('content'));
                                $msg= "success creating and writing to file";
			     }
                             catch (Exception $e){
                                $written=false;
                               $msg= "Error writing to file";
                           }
                        }
			$details=array(
			'status'=>$written,
		        'dir'=>  basename (dirname($filetobesaved),"/"),
                        'title'=>basename($filetobesaved),
		        'path'=>dirname($filetobesaved),
		        'msg'=>$msg
	        );		
	       echo json_encode($details);
   }
   
   function check_syntax()
   {
	$path=get_policiesdir().$this->session->userdata('username').'/promises.cf';
	if(file_exists($path))
		 $result=cfpr_validate_policy($path);
	else
		$result="Failed to find promises.cf";
	$details=array(
	        'result'=>$result
			);
	echo json_encode($details);
   }

 }