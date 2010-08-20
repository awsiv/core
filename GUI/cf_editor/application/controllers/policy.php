<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Policy extends Controller {

	
	function Policy()
	{
		parent::Controller();
		$this->load->helper('url');	
		$this->load->model('db_policy');
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
		$data['policies']=$this->db_policy->get_policies();
		$this->load->view('policy_list',$data);
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
	
	
}
?>
