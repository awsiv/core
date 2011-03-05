<?php defined('BASEPATH') OR exit('No direct script access allowed');

if ( ! class_exists('Controller'))
{
	class Controller extends CI_Controller {}
}

class Auth extends Controller {

	function __construct()
	{
		parent::__construct();
		$this->load->library(array('ion_auth','ion_auth','form_validation','breadcrumb','breadcrumblist'));
                //list of errors wrapped in <p> class of errors
                $this->form_validation->set_error_delimiters('<span class="errorlist">', '</span>');
		//$this->load->database();
		$this->load->helper('url');
		$this->config->load('ion_auth',TRUE);
	}

	//redirect if needed, otherwise display the user list
	function index()
	{
		if (!$this->ion_auth->logged_in())
		{
			//redirect them to the login page
			redirect('auth/login', 'refresh');
		}
		elseif (!$this->ion_auth->is_admin())
		{
			//redirect them to the home page because they must be an administrator to view this
			redirect($this->config->item('base_url'), 'refresh');
		}
		else
		{
			//set the flash data error message if there is one
			//$identifier=$this->config->item('identity','ion_auth');
 
                        $this->data['title']="Cfengine Mission Portal - Admin";
                        $this->data['title_header']="Admin";
                        $this->data['admin']="current";
			$this->data['username'] = $this->session->userdata('username');
			$this->data['message'] = (validation_errors()) ? '<p class="error">'.validation_errors().'</p>' : $this->session->flashdata('message');
			//list the users
			$this->data['users'] = $this->ion_auth->get_users_array();
			//$this->load->view('auth/index', $this->data);
                        if(is_ajax ())
                        {
                         $this->load->view('auth/user_list',$this->data);
                            //$this->load->view('welcome/index',$this->data);
                        }
                        else
                        {
                        //$this->template->load('template', 'auth/index',$this->data);
                        redirect($this->config->item('base_url'), 'refresh');
                        }
		}
	}

        function admin_page()
        {
            $bc = array(
            'title' => 'Admin',
            'url' => 'auth/admin_page',
            'isRoot' => false
           );
             $this->breadcrumb->setBreadCrumb($bc);
             $this->data['title']="Cfengine Mission Portal - Admin";
                        $this->data['title_header']="Admin";
                        $this->data['admin']="current";
			$this->data['username'] = $this->session->userdata('username');
			$this->data['message'] = (validation_errors()) ? '<p class="error">'.validation_errors().'</p>' : $this->session->flashdata('message');
			//list the users
			$this->data['users'] = $this->ion_auth->get_users_array();
			//$this->load->view('auth/index', $this->data);
                        $this->data['breadcrumbs'] = $this->breadcrumblist->display();
                        if(is_ajax ())
                        {
                         $this->load->view('auth/user_list',$this->data);
                            //$this->load->view('welcome/index',$this->data);
                        }
                        else
                        {
                        $this->template->load('template', 'auth/index',$this->data);
                        }
            
        }

	//log the user in
	function login()
	{
		$this->data['title'] = "Login";
                $identifier=$this->config->item('identity','ion_auth');
		//validate form input
		if($identifier=="username")
		   $this->form_validation->set_rules('username', 'User Name', 'required');
		else
		   $this->form_validation->set_rules('email', 'Email Address', 'required|valid_email');
		
		$this->form_validation->set_rules('password', 'Password', 'required');

		if ($this->form_validation->run() == true)
		{ //check to see if the user is logging in
			//check for "remember me"
			$remember = (bool) $this->input->post('remember');

			if ($this->ion_auth->login($this->input->post($identifier), $this->input->post('password'), $remember))
			{ //if the login is successful
				//redirect them back to the home page
				$this->session->set_flashdata('message', $this->ion_auth->messages());
				//redirect($this->config->item('base_url'), 'refresh');
				redirect('auth/index','refresh');
			}
			else
			{ //if the login was un-successful
				//redirect them back to the login page
				$this->session->set_flashdata('message', $this->ion_auth->errors());
				redirect('auth/login', 'refresh'); //use redirects instead of loading views for compatibility with MY_Controller libraries
			}
		}
		else
		{  //the user is not logging in so display the login page
			//set the flash data error message if there is one
			$this->data['message'] = (validation_errors()) ? validation_errors() : $this->session->flashdata('message');
			
            if($identifier=="username"){
			    $this->data['lbl_identifier']="Username";
				$this->data['identifier'] = array('name' => 'username',
				'id' => 'username',
				'type' => 'text',
				'value' => $this->form_validation->set_value('username'),
			);
			}
			else{
				$this->data['lbl_identifier']="Email";
				$this->data['identifier'] = array('name' => 'email',
				'id' => 'email',
				'type' => 'text',
				'value' => $this->form_validation->set_value('email'),
			);
			}
			
			
			$this->data['password'] = array('name' => 'password',
				'id' => 'password',
				'type' => 'password',
			);

			$this->load->view('auth/login', $this->data);
		}
	}

	//log the user out
	function logout()
	{
		$this->data['title'] = "Logout";

		//log the user out
		$logout = $this->ion_auth->logout();

		//redirect them back to the page they came from
		redirect('auth', 'refresh');
	}

	//change password
	function change_password($id=Null)
	{
		$this->form_validation->set_rules('old', 'Old password', 'required');
		$this->form_validation->set_rules('new', 'New Password', 'required|min_length[' . $this->config->item('min_password_length', 'ion_auth') . ']|max_length[' . $this->config->item('max_password_length', 'ion_auth') . ']|matches[new_confirm]');
		$this->form_validation->set_rules('new_confirm', 'Confirm New Password', 'required');

		if (!$this->ion_auth->logged_in())
		{
			redirect('auth/login', 'refresh');
		}
		//$user = $this->ion_auth->get_user($this->session->userdata('user_id'));

		if ($this->form_validation->run() == false)
		{ //display the form
			//set the flash data error message if there is one
			$this->data['message'] = (validation_errors()) ? validation_errors() : $this->session->flashdata('message');

			$this->data['old_password'] = array('name' => 'old',
				'id' => 'old',
				'type' => 'password',
                                'value' => $this->form_validation->set_value('old')
			);
			$this->data['new_password'] = array('name' => 'new',
				'id' => 'new',
				'type' => 'password',
                                'value' => $this->form_validation->set_value('new')
			);
			$this->data['new_password_confirm'] = array('name' => 'new_confirm',
				'id' => 'new_confirm',
				'type' => 'password',
                                'value' => $this->form_validation->set_value('new_confirm')
			);
			//render
			$this->load->view('auth/change_password', $this->data);
		}
		else
		{
			$identity = isset($id)?$id:$this->session->userdata($this->session->userdata('user_id'));
			$change = $this->ion_auth->change_password($identity, $this->input->post('old'), $this->input->post('new'));

			if ($change)
			{ //if the password was successfully changed
				$this->session->set_flashdata('message', $this->ion_auth->messages());
				//$this->logout();
                                redirect('auth/index', 'refresh');
			}
			else
			{
				$this->session->set_flashdata('message', $this->ion_auth->errors());
				redirect('auth/change_password', 'refresh');
			}
		}
	}

	//forgot password
	function forgot_password()
	{
		$this->form_validation->set_rules('email', 'Email Address', 'required|valid_email');
		if ($this->form_validation->run() == false)
		{
			//setup the input
			$this->data['email'] = array('name' => 'email',
				'id' => 'email',
			);
			//set any errors and display the form
			$this->data['message'] = (validation_errors()) ? validation_errors() : $this->session->flashdata('message');
			$this->load->view('auth/forgot_password', $this->data);
		}
		else
		{
			//run the forgotten password method to email an activation code to the user
			$forgotten = $this->ion_auth->forgotten_password($this->input->post('email'));

			if ($forgotten)
			{ //if there were no errors
				$this->session->set_flashdata('message', $this->ion_auth->messages());
				redirect("auth/login", 'refresh'); //we should display a confirmation page here instead of the login page
			}
			else
			{
				$this->session->set_flashdata('message', $this->ion_auth->errors());
				redirect("auth/forgot_password", 'refresh');
			}
		}
	}

	//reset password - final step for forgotten password
	public function reset_password($code)
	{
		$reset = $this->ion_auth->forgotten_password_complete($code);

		if ($reset)
		{  //if the reset worked then send them to the login page
			$this->session->set_flashdata('message', $this->ion_auth->messages());
			redirect("auth/login", 'refresh');
		}
		else
		{ //if the reset didnt work then send them back to the forgot password page
			$this->session->set_flashdata('message', $this->ion_auth->errors());
			redirect("auth/forgot_password", 'refresh');
		}
	}

	//activate the user
	function activate($id, $code=false)
	{
		$activation = $this->ion_auth->activate($id, $code);

		if ($activation)
		{
			//redirect them to the auth page
			$this->session->set_flashdata('message', $this->ion_auth->messages());
			redirect("auth", 'refresh');
		}
		else
		{
			//redirect them to the forgot password page
			$this->session->set_flashdata('message', $this->ion_auth->errors());
			redirect("auth/forgot_password", 'refresh');
		}
	}

	//deactivate the user
	function deactivate($id = NULL)
	{
		// no funny business, force to integer
		$id = (int) $id;

		$this->load->library('form_validation');
		$this->form_validation->set_rules('confirm', 'confirmation', 'required');
		$this->form_validation->set_rules('id', 'user ID', 'required|is_natural');

		if ($this->form_validation->run() == FALSE)
		{
			// insert csrf check
			$this->data['csrf'] = $this->_get_csrf_nonce();
			$this->data['user'] = $this->ion_auth->get_user($id);
			$this->load->view('auth/deactivate_user', $this->data);
		}
		else
		{
			// do we really want to deactivate?
			if ($this->input->post('confirm') == 'yes')
			{
				// do we have a valid request?
				if ($this->_valid_csrf_nonce() === FALSE || $id != $this->input->post('id'))
				{
					show_404();
				}

				// do we have the right userlevel?
				if ($this->ion_auth->logged_in() && $this->ion_auth->is_admin())
				{
					$this->ion_auth->deactivate($id);
				}
			}

			//redirect them back to the auth page
			redirect('auth', 'refresh');
		}
	}

	//create a new user
	function create_user()
	{
		$this->data['title'] = "Create User";

		if (!$this->ion_auth->logged_in() || !$this->ion_auth->is_admin())
		{
			redirect('auth', 'refresh');
		}

		//validate form input
		$this->form_validation->set_rules('user_name', 'User Name', 'required|xss_clean|unique[users.username]');
		$this->form_validation->set_rules('email', 'Email Address', 'required|valid_email|unique[users.email]');
		$this->form_validation->set_rules('password', 'Password', 'required|min_length[' . $this->config->item('min_password_length', 'ion_auth') . ']|max_length[' . $this->config->item('max_password_length', 'ion_auth') . ']|matches[password_confirm]');
		$this->form_validation->set_rules('password_confirm', 'Password Confirmation', 'required');
		$this->form_validation->set_rules('group[]', 'Group', 'required|xss_clean');

		if ($this->form_validation->run() == true)
		{
			$username = strtolower($this->input->post('user_name'));
			$email = $this->input->post('email');
			$password = $this->input->post('password');
			$group=$this->input->post('group');
			/*$additional_data = array('first_name' => $this->input->post('first_name'),
				'last_name' => $this->input->post('last_name'),
				'company' => $this->input->post('company'),
				'phone' => $this->input->post('phone1') . '-' . $this->input->post('phone2') . '-' . $this->input->post('phone3'),
				'group_id'=>$this->input->post('group')
			);*/
		}
		if ($this->form_validation->run() == true && $this->ion_auth->register($username, $password, $email, $group))
		{ //check to see if we are creating the user
			//redirect them back to the admin page
                    if(is_ajax ())
                    {
                    $this->data['message'] =$this->ion_auth->messages();
                    $this->data['users'] = $this->ion_auth->get_users_array();
                    $this->load->view('auth/user_list',$this->data);
                    }
                    else
                    {
                    $this->session->set_flashdata('message', "User Created");
                    redirect("auth", 'refresh');
                    }
		}
		else
		{ //display the create user form
			//set the flash data error message if there is one
                        
			$this->data['message'] = (validation_errors() ?validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));

                       $this->data['user_name'] = array('name' => 'user_name',
				'id' => 'user_name',
				'type' => 'text',
				'value' => $this->form_validation->set_value('user_name'),
			);
			
			$this->data['email'] = array('name' => 'email',
				'id' => 'email',
				'type' => 'text',
				'value' => $this->form_validation->set_value('email'),
			);
			$this->data['password'] = array('name' => 'password',
				'id' => 'password',
				'type' => 'password',
				'value' => $this->form_validation->set_value('password'),
			);
			$this->data['password_confirm'] = array('name' => 'password_confirm',
				'id' => 'password_confirm',
				'type' => 'password',
				'value' => $this->form_validation->set_value('password_confirm'),
			);
			
			/* changing the data obtained from database into
			 * key value pair used in select element
			 */
			 
			/*$options =array();
            $groups=$this->ion_auth->get_groups();
			foreach ($groups as $group)
			{
				$options[$group['id']]=$group['name'];
			}*/
                        $groups=$this->ion_auth->get_groups();
			foreach ($groups as $group)
	                {
                         $this->data['groups'][$group['name']]=array('name'=>'group[]',
                                                             'id'=>$group['name'],
                                                             'value'=>$group['name'],
                                                             'checked'=>$this->form_validation->set_checkbox('group[]',$group['name'])
                                                             );
                        }
			//$this->data['group']=array('name'=>'group','options'=>$options,'default'=>set_value('group', '2'));
                        $this->load->view('auth/create_user', $this->data);
		}
	}

	function _get_csrf_nonce()
	{
		$this->load->helper('string');
		$key = random_string('alnum', 8);
		$value = random_string('alnum', 20);
		$this->session->set_flashdata('csrfkey', $key);
		$this->session->set_flashdata('csrfvalue', $value);

		return array($key => $value);
	}

	function _valid_csrf_nonce()
	{
		if ($this->input->post($this->session->flashdata('csrfkey')) !== FALSE &&
				$this->input->post($this->session->flashdata('csrfkey')) == $this->session->flashdata('csrfvalue'))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	
	function edit_user($id)
	{
		$this->data['title'] = "Edit User";

		if (!$this->ion_auth->logged_in() || !$this->ion_auth->is_admin())
		{
			redirect('auth', 'refresh');
		}
		//validate form input
		$this->form_validation->set_rules('user_name', 'First Name', 'required|xss_clean');
		$this->form_validation->set_rules('email', 'Email Address', 'required|valid_email');
		$this->form_validation->set_rules('group[]', 'Group', 'required|xss_clean');
		
		if ($this->form_validation->run() == true)
		{
			$data = array(
				'username' => $this->input->post('user_name'),
                                'email' => $this->input->post('email'),
				'group'=>$this->input->post('group'),
			);
		}
		if ($this->form_validation->run() == true && $this->ion_auth->update_user($id, $data))
		{ //check to see if we are creating the user
			//redirect them back to the admin page
                    if(is_ajax ())
                    {
                    $this->data['message'] =$this->ion_auth->messages();
                    $this->data['users'] = $this->ion_auth->get_users_array();
                    $this->load->view('auth/user_list',$this->data);
                    }
                    else
                    {
                    $this->session->set_flashdata('message', "User Updated");
                    redirect("auth", 'refresh');
                    }
		}
		else
		{ //display the create user form
			//set the flash data error message if there is one
			$this->data['message'] = (validation_errors() ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));
			$user=$this->ion_auth->get_user($id);
                        //$phone=explode('-',$user->phone);
			$this->data['user_name'] = array('name' => 'user_name',
				'id' => 'first_name',
				'type' => 'text',
				'value' => $this->form_validation->set_value('user_name',$user->username),
			);
			
			$this->data['email'] = array('name' => 'email',
				'id' => 'email',
				'type' => 'text',
				'value' => $this->form_validation->set_value('email',$user->email),
			);
			
                        $groups=$this->ion_auth->get_groups();
			foreach ($groups as $group)
	                {
                         $this->data['groups'][$group['name']]=array('name'=>'group[]',
                                                             'id'=>$group['name'],
                                                             'value'=>$group['name'],
                                                             'checked'=>$this->form_validation->set_checkbox('group[]',$group['name'],(in_array($group['name'],$user->group)) ? TRUE : FALSE)
                                                            );
                        }
			
			//$this->data['group']=array('name'=>'group','options'=>$options,'default'=>set_value('group', $user->group_id));
			$this->load->view('auth/edit_user', $this->data);
		}
		
	}
	
	function manage_group($op=false,$id=false)
	{
	if (!$this->ion_auth->logged_in() || !$this->ion_auth->is_admin())
			{
				redirect('auth', 'refresh');
			}
	 if(!empty($op))
	 {
		        $this->data['title'] = "Create Group";
			$this->data['operation']="Create";
                        if($op!='edit')
			   $this->form_validation->set_rules('name', 'Name', 'required|xss_clean|unique[groups.name]');
                       else
                           $this->form_validation->set_rules('name', 'Name', 'required|xss_clean');
		     $this->form_validation->set_rules('description', 'Description', 'required|xss_clean');
			if ($this->form_validation->run() == true)
			{
				$data=array('name'=>$this->input->post('name'),'description'=>$this->input->post('description'));
				 if(($op=='edit' && !$this->ion_auth->update_group($id,$data)))
				  {
                                    $this->__load_group_add_edit($op,$id);
                                    return;
				  }
                                  if($op=='create' && $this->ion_auth->create_group($data)=="")
                                  {
                                    $this->__load_group_add_edit($op,$id);
                                    return;
                                  }
                                 if(is_ajax ())
                                        {
                                         $this->data['message'] =$this->ion_auth->messages();
                                         $this->data['groups']=$this->ion_auth->get_groups();
                                         $this->load->view('auth/list_group',$this->data);
                                        }
                                        else
                                        {
                                            $this->session->set_flashdata('message', $this->ion_auth->messages());
                                            redirect("auth/manage_group", 'refresh');
                                        }
			}
			else
			{
				 $this->__load_group_add_edit($op,$id);
			}
	  }
	  else
	  {
		 $this->data['message'] = (validation_errors()) ? validation_errors() : $this->session->flashdata('message');
		 $this->data['groups']=$this->ion_auth->get_groups();
		 $this->load->view('auth/list_group',$this->data);
	  }
	}

        function __load_group_add_edit($op,$id)
        {
            $this->data['message'] = (validation_errors() ? validation_errors() : ($this->ion_auth->errors() ? $this->ion_auth->errors() : $this->session->flashdata('message')));
				$this->data['name'] = array('name' => 'name',
					'id' => 'name',
					'type' => 'text',
					'value' => $this->form_validation->set_value('name'),
				);
				$this->data['description'] = array('name' => 'description',
					'id' => 'last_name',
					'type' => 'text',
					 'rows'  => '3',
					 'cols'  => '20',
					'value' => $this->form_validation->set_value('description'),
				);
			   if($op=='edit')
			    {
				 if(empty($id))
				 {
					show_error('cannot update - record not specified' ,  500);
					return;
				 }
			     $group=$this->ion_auth->get_group($id);
			     $this->data['title'] = "Edit Group";
			     $this->data['operation']="Update";
		             $this->data['name']['enable']='enable';
		             $this->data['name']['value']=$this->form_validation->set_value('name',$group->name);
			     $this->data['description']['value'] = $this->form_validation->set_value('description',property_exists($group, 'description')?$group->description:"");
			    }
             $this->load->view('auth/add_edit_group', $this->data);
        }

        function delete_user($id)
        {
            $this->ion_auth->delete_user($id);
            if(is_ajax ())
            {
            $this->data['message'] = $this->ion_auth->messages();
	    $this->data['users'] = $this->ion_auth->get_users_array();
            $this->load->view('auth/user_list',$this->data);
            }
            else
            {
             $this->session->set_flashdata('message', $this->ion_auth->messages());
             redirect("auth", 'refresh');
            }
         
        }

        function delete_group($id)
        {
           $this->ion_auth->delete_group($id);
           if(is_ajax ())
            {
            $this->data['message'] = $this->ion_auth->messages();
	    $this->data['groups'] = $this->ion_auth->get_groups();
            $this->load->view('auth/list_group',$this->data);
            }
            else
            {
             $this->session->set_flashdata('message', $this->ion_auth->messages());
             redirect("auth/manage_group", 'refresh');
            }
        }

}
