<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Policy extends Controller {

	
	function Policy()
	{
		parent::Controller();
		$this->load->helper('url');	
		$this->load->model('db_policy');
		$this->load->helper('file');
	}
	
	
	//for adding the new file ,it handles the posted data to model and returns json data to the respective view.
	function add()
	{
		$data=array(
		'policy_name'=> $_POST['name'],
		'policy_text'=> $_POST['content']
		);
		$created_id=$this->db_policy->add_policy($data);
		if($created_id)
		{
			$data['policy']=$this->db_policy->get_policy_details($created_id);
	        $details=array(
		    'title'=>$data['policy']->policy_name,
		    'id'=>$created_id,
	        );
	     echo json_encode($details);
		}
			
	}
	
	//get list of policies file in database
	function get_list()
	{
	//$data['policies']=$this->db_policy->get_policies();
	//$this->load->view('policy_list',$data);
	$root='./';
	$_POST['dir'] = urldecode($_POST['dir']);
	    if(file_exists($root . $_POST['dir']) ) {
	    $files = scandir($root . $_POST['dir']);
	    natcasesort($files);
		if( count($files) > 2 ) { /* The 2 accounts for . and .. */
			echo "<ul id=\"policies_list_new\" class=\"jqueryFileTree\">";
			// All dirs
			foreach( $files as $file ) {
				if( file_exists($root . $_POST['dir'] . $file) && $file != '.' && $file != '..' && is_dir($root . $_POST['dir'] . $file) ) {
					echo "<li class=\"directory collapsed\"><a href=\"#\" rel=\"" . htmlentities($_POST['dir'] . $file) . "/\">" . htmlentities($file) . "</a></li>";
				}
			}
			// All files
			$i=1;
			foreach( $files as $file ) {
				if( file_exists($root . $_POST['dir'] . $file) && $file != '.' && $file != '..' && !is_dir($root . $_POST['dir'] . $file) ) {
					$ext = preg_replace('/^.*\./', '', $file);
					echo "<li class=\"file ext_$ext\"><a href=\"#\" rel=\"" . htmlentities($_POST['dir'] . $file) . "\" id=\"policy_".$i."\">" . htmlentities($file) .  "</a></li>";
					$i++;
				}
			}
			echo "</ul>";	
		}
      }
	}
	
	function get_file_contents()
	{
		$file_path=$_POST['file_path'];
		$filename= basename($file_path);
		$contents = read_file($_POST['file_path']);
		$information=get_file_info($file_path);
		
		$details=array(
		'filename'=>$filename,
		'content'=>$contents,
		'information'=>$information,
	     );
	    echo json_encode($details);
	}
	
   function compare_file_contents()
   {
   	    $status="unchanged";
  		$file_path=$_POST['file_path'];
		$contents = read_file($_POST['file_path']);
		$newcontents=str_replace('\u200b', '' , $_POST['newcontents']);
		if($_POST['agent']=='webkit')
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
	
	function save_file_contents()
	{
		//$file=substr($_POST['file'], strrpos($filename, '.')
		$filetobesaved='./jqueryFileTree/'.$_POST['file'];
		if ( ! write_file($filetobesaved, $_POST['content']))
		{
		    $details=array(
		    'title'=>$_POST['file'],
		    'path'=>$filetobesaved,
		    'msg'=>"unable to write file to disk"
	        );
	       echo json_encode($details);
		}
		else
		{
		     $details=array(
		    'title'=>$_POST['file'],
		    'path'=>'jqueryFileTree/'.$_POST['file'],
		     'msg'=>"File written!!!"
	        );
	        echo json_encode($details);
		}
	
	}
	
	//get details of a policy 
	function get_policy($pid)
	{
	$data['policy']=$this->db_policy->get_policy_details($pid);
	$details=array(
		'title'=>$data['policy']->policy_name,
		'content'=>$data['policy']->policy_text,
	);
	echo json_encode($details);
	}
	
	//update the policy specified with the  new contents supplied.
	function update($pid)
	{
	$data=array(
		'policy_text'=> $_POST['content']
		);
		$updated=$this->db_policy->update_policy($pid,$data);
		if($updated)
		{
			$data['policies']=$this->db_policy->get_policies();
			$this->load->view('policy_list',$data);
		}
	}
	
	//function for checking the synatx of the code supplied.In future it should include compilation techniques, for synatax checking and 
	//and provide the result accordingly.
	function check_syntax()
	{
		$data=array(
		'policy_name'=> $_POST['name'],
		'policy_text'=> $_POST['content'],
		'result'=>'successfull',
		'details'=>'Syntax compiled successfully'
		);
		echo json_encode($data);
	}
	
	function test()
	{
	$this->load->view('welcome_message');	
	}
	
	function missioncontrol()
	{
	$this->load->view('missioncontrol');	
	}
	
	function loadbundleview()
	{
	$this->load->view('createbundle');	
	}
	
	
}
?>
