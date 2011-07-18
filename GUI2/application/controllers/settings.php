<?php
class Settings extends Cf_Controller {
 function __construct() {
        parent::__construct();
        $this->load->library('form_validation');
        $this->load->helper('form');
	$this->load->helper('url');
        $this->load->model('settings_model');
    }


    function manage($op=false, $id=false){
                $required_if_ldap =$this->__ldap_check();
                $required_if_database=$this->__db_check();
                $this->form_validation->set_rules('appemail', 'Application email', 'xss_clean|trim|required|valid_email');
                $this->form_validation->set_rules('mode', 'Authentication mode', 'xss_clean|trim|required');
		$this->form_validation->set_rules('username', 'username', 'xss_clean|trim'.$required_if_database);
		$this->form_validation->set_rules('password', 'password', 'xss_clean|trim'.$required_if_database.'|matches[confirm_password]');
		$this->form_validation->set_rules('confirm_password', 'confirm password', 'trim'.$required_if_database);
		$this->form_validation->set_rules('host', 'host', 'xss_clean|trim'. $required_if_ldap);
		$this->form_validation->set_rules('base_dn', 'base dn', 'xss_clean|trim'. $required_if_ldap);
		$this->form_validation->set_rules('login_attribute', 'login attribute', 'xss_clean|trim'. $required_if_ldap);
		$this->form_validation->set_rules('active_directory', 'Active directory', 'xss_clean|trim'. $required_if_ldap);
		$this->form_validation->set_rules('active_directory_domain', 'active directory domain', 'xss_clean|trim'. $required_if_ldap);
		$this->form_validation->set_rules('member_attribute', 'member attribute', 'xss_clean|trim'. $required_if_ldap);

		$this->form_validation->set_error_delimiters('<br /><span>', '</span>');

		if ($this->form_validation->run() == FALSE) // validation hasn't been passed
		{
                         $bc = array(
                                'title' => 'Admin',
                                'url' => 'auth/setting',
                                'isRoot' => false,
                                'replace_existing' => true
                            );
                        $this->breadcrumb->setBreadCrumb($bc);
                        $data = array(
                            'title' => "Cfengine Mission Portal - authentication",
                            'breadcrumbs' => $this->breadcrumblist->display(),
                            'message'=>validation_errors(),
                            'op'=>'create'
                        );                      
		      
                      //if previous settings exist load it and display
                       $settings= $this->settings_model->get_app_settings();
                        if (is_object($settings))// the information has therefore been successfully saved in the db
                        {
                          foreach($settings as $property=>$value){
                              if($property=='mode' ||$property=='active_directory')
                              {
                                $data[$value]=' checked="checked"';
                                 continue;
                              }
                              if($property != '_id'){
                                 $data[$property]=$this->form_validation-> set_value($property,$value);
                              }
                            }
                           $data['op']='edit';
                         }
                      $this->template->load('template', 'appsetting/missionportalpref', $data);
		}
		else // passed validation proceed to post success logic
		{
		 	// build array for the model
			$form_data = array(
                                                'appemail'=>set_value('appemail'),
					       	'mode' => set_value('mode'),
					       	'username' => set_value('username'),
					       	'password' => set_value('password'),
					       	'confirm_password' => set_value('confirm_password'),
					       	'host' => set_value('host'),
					       	'base_dn' => set_value('base_dn'),
					       	'login_attribute' => set_value('login_attribute'),
					       	'active_directory' => set_value('active_directory'),
					       	'active_directory_domain' => set_value('active_directory_domain'),
					       	'member_attribute' => set_value('member_attribute')
						);
                       
			// run insert model to write data to db
                        $inserted='';
                        if($op=='edit')
                        {
                        $settings= $this->settings_model->get_app_settings();
                        $inserted=$this->settings_model->update_app_settings($form_data,$settings->_id->__toString());
                        }
                        else
                        {
                          $inserted=$this->settings_model->insert_app_settings($form_data);
                        }
                  if ($inserted)// the information has therefore been successfully saved in the db
			  {
			   //redirect('settings/success');   // or whatever logic needs to occur
                         $bc = array(
                                'title' => 'Admin',
                                'url' => 'auth/setting',
                                'isRoot' => false,
                                'replace_existing' => true
                            );
                        $this->breadcrumb->setBreadCrumb($bc);
                        $data = array(
                            'title' => "Cfengine Mission Portal - Settings",
                            'breadcrumbs' => $this->breadcrumblist->display(),
                            'op'=>'edit',
                            'message'=>'<p class="success"> Settings configured sucessfully</p>'
                        );
                         foreach($form_data as $property=>$value){
                              if($property=='mode')
                              {
                                $data[$value]=' checked="checked"';
                                  continue;

                              }
                              if($property != '_id'){
                                 $data[$property]=$this->form_validation-> set_value($property,$value);
                              }
                          }
		      //$this->load->view('appsetting/missionportalpref',$data);
                      $this->template->load('template', 'appsetting/missionportalpref', $data);

			}
			else
			{
			echo 'An error occurred saving your information. Please try again later';
			// Or whatever error handling is necessary
			}
		}
    }

    function __ldap_check()
    {

         if($this->input->post('mode')&&strtolower($this->input->post('mode'))=='ldap')
         {
           return '|required';
         }
         else
         {
             return '';
         }
    }

    function __db_check(){
        if($this->input->post('mode')&&strtolower($this->input->post('mode'))=='database')
         {
           return '|required';
         }
         else
         {
             return '';
         }
    }
}
?>
