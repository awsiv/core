<?php
 class Cfeditor extends Cf_Controller {
  
  function Cfeditor()
   {
   parent::__construct();
   $this->load->library('jcryption');
   //session_start();
   }
  
   function index()
   {
   $data=array(
         'title'=>"Cfengine Mission Portal - Policy editor",
        // 'title_header'=>"Policy editor",
	// 'injected_item'=>'<link href="'.get_cssdir().'jquery-ui-1.8.2.custom.css" rel="stylesheet" media="screen" />'
		 );
   $this->carabiner->css('cfeditor.css');
    $this->carabiner->js('jqueryFileTree.js');
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
    $data=$this->cfsvn->cfsvn_checkout();
    echo json_encode($data);	
   }
   
   function commit()
   {
     $working_dir=get_policiesdir().$this->session->userdata('username');
     if(!$this->input->post('file'))
		 {
         $working_dir=$working_dir.'/'.$this->input->post('file');
		 }
     $password=$this->jcryption->decrypt($this->input->post('passwd'),$_SESSION["d"]["int"],$_SESSION["n"]["int"]);
     $params=array(
	        'username' =>  $this->input->post('user'),
			'password' => $password,
			'repository' => $this->input->post('repo'),
			'workingdir' => $working_dir
			);   
	 $this->load->library('cfsvn',$params);
     $cdetails=$this->cfsvn->cfsvn_commit($this->input->post('comments'));	
     echo json_encode ($cdetails);
   }
   
   function update()
   {
     $password=$this->jcryption->decrypt($this->input->post('passwd'),$_SESSION["d"]["int"],$_SESSION["n"]["int"]);
     $params=array(
	        'username' =>  $this->input->post('user'),
			'password' => $password,
			'workingdir' => get_policiesdir().$this->session->userdata('username')
			);   
	 $this->load->library('cfsvn',$params);
     $cdetails=$this->cfsvn->cfsvn_update();
     echo $cdetails;
   }
   
   function svnlogs()
   {
   $no_of_records=50;
   $password=$this->jcryption->decrypt($this->input->post('passwd'),$_SESSION["d"]["int"],$_SESSION["n"]["int"]);
   $params=array(
	        'username' =>  $this->input->post('user'),
			'password' => $password,
			'repository' => $this->input->post('repo'),
			);
	$this->load->library('cfsvn',$params);
	$data=array('logs'=>$this->cfsvn->cfsvn_log($no_of_records));
	$logs=array(
			'status'=>'success',
			'table'=>$this->load->view('cfeditor/svnlogs',$data,true)
			 );
	echo json_encode ($logs);
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
				$handle = fopen($filetobesaved, 'w');
				if(!$handle)
				{
					$msg="couldn't open file <i>$filetobesaved</i>";
				}
			    $content=str_replace('\\\\', '\\' , $this->input->post('content'));
//			    $content=str_replace('\\"', '"' , $content);
                $content=str_replace('&gt;', '>' , $content);
				if($_POST['agent']=='webkit')
				{
				$content=substr($content, 0, -3);
				}
				fwrite($handle, $content);
				$msg= "success writing to file,".$content;
				$written=true;
				fclose($handle);
			}
			elseif(file_exists($filetobesaved)&&$this->input->post('filestats')=='new')
			{
				$msg= "couldn't write file <i>$filetobesaved</i>,File with same name exist in repository";
			}
			else
			{
				$handle = fopen($filetobesaved, 'w');
				if(!$handle)
				{
					$msg="couldn't open file <i>$filetobesaved</i>";
				}
			    $content=str_replace('\\\\', '\\' , $this->input->post('content'));

				$content=str_replace('&gt;', '>' , $content);
				if($_POST['agent']=='webkit')
				{
				$content=substr($content, 0, -3);
				}
				fwrite($handle, $content);
				$written=true;
				$msg= "success writing to file";
				fclose($handle);
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