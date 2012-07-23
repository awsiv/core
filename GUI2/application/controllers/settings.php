<?php

class Settings extends Cf_Controller
{

    function __construct()
    {
        parent::__construct();
        $this->load->library('form_validation');
        $this->load->library('ion_auth');
        $this->load->helper('form');
        $this->load->helper('url');
        $this->load->model('settings_rest_model');
        $this->settings_rest_model->setRestClient($this->ion_auth->getRestClient());
    }


    function index() {
        $bc = array(
            'title' => $this->lang->line('breadcrumb_setting'),
            'url' => 'settings/index',
            'isRoot' => false,
            'directchild' => true,
            'replace_existing' => true,
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - Settings",
            'breadcrumbs' => $this->breadcrumblist->display(),
            "is_displayable" => $this->ion_auth->is_accessible()
        );
        $this->template->load('template', 'auth/usersettings', $data);
    }

    function manage()
    {

        $requiredjs = array(
            array('widgets/notes.js'),
        );
        $this->carabiner->js($requiredjs);

        if (!$this->ion_auth->is_accessible())
        {
            redirect('settings/index');
        }

        try
        {
            $settingsValue = $this->settings_rest_model->get_app_settings();
        }
        catch (Exception $e)
        {
            // for now set the setting value as empty array whn error
            show_error($e->getMessage());
        }


        $bc = array(
            'title' => $this->lang->line('breadcrumb_setting'),
            'url' => 'settings/manage',
            'isRoot' => false,
            'directchild' => true,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $required_if_ldap = $this->__ldap_check();
        $required_if_ad = $this->__AD_check();

        $this->form_validation->set_rules('appemail', 'Administrative email', 'xss_clean|trim|required|valid_email');
        $this->form_validation->set_rules('mode', 'Authentication mode', 'xss_clean|trim|required');
        $this->form_validation->set_rules('rbac', 'Role based acccess control', 'xss_clean|trim|required');
        $this->form_validation->set_rules('host', 'host', 'xss_clean|trim' . $required_if_ldap . $required_if_ad);
        $this->form_validation->set_rules('base_dn', 'base dn', 'xss_clean|trim' . $required_if_ldap . $required_if_ad);
        $this->form_validation->set_rules('login_attribute', 'login attribute', 'xss_clean|trim' . $required_if_ldap);
        $this->form_validation->set_rules('user_directory[]', 'users directory', 'xss_clean' . $required_if_ldap);
        $this->form_validation->set_rules('active_directory_domain', 'active directory domain', 'xss_clean|trim' . $required_if_ad);
        $this->form_validation->set_rules('external_admin_username', 'External admin user name', 'xss_clean|trim' . $required_if_ldap . $required_if_ad);
        $this->form_validation->set_rules('encryption', 'Encryption mode', 'xss_clean|trim' . $required_if_ldap . $required_if_ad);
        $this->form_validation->set_rules('bluehost_threshold_global', 'Blue host horizon', 'callback_validate_bluehost_threshold');
        $this->form_validation->set_error_delimiters('<span>', '</span><br/>');
        if ($this->form_validation->run() == true)
        {
            $returnData = $this->_updateData();
            if (is_array($returnData) && $returnData['status'])
            {
                $this->session->set_flashdata('message', array('content' => $returnData['message'], 'type' => 'success'));
                //refresh
                redirect('settings/manage');
            }
            else
            {
                //$this->session->set_flashdata('message', array('content' => $returnData['message'], 'type' => 'error'));
                $feedback = " <p class=\"error\">" .$returnData['message']."</p>";
                //redirect('settings/manage');
            }
            
            
        } else {
            $feedback = validation_errors();
        }




        $bluehost_threshold_min = set_value('bluehost_threshold_global', $settingsValue['blueHostHorizon'] / 60);


        $userDirectory = $this->input->post('user_directory') ? $this->input->post('user_directory') : explode(';', $settingsValue['ldapUsersDirectory']);
        $userDirectory=array_filter($userDirectory);
        $form_data = array(
            'appemail' => set_value('appemail', $settingsValue['appemail']),
            'mode' => set_value('mode', $settingsValue['authMode']),
            'host' => set_value('host', $settingsValue['ldapHost']),
            'base_dn' => set_value('base_dn', $settingsValue['ldapBaseDN']),
            'login_attribute' => set_value('login_attribute', $settingsValue['ldapLoginAttribute']),
            'active_directory_domain' => set_value('active_directory_domain'),
            'encryption' => set_value('encryption', $settingsValue['ldapEncryption']),
            'external_admin_username' => set_value('external_admin_username'),
            'rbac' => set_value('rbac', $settingsValue['rbac']),
            'bluehost_threshold_global' => set_value('bluehost_threshold_global', $bluehost_threshold_min),
            'user_dirs' => $userDirectory,
        );
        $data = array(
            'title' => "CFEngine Mission Portal - Settings",
            'breadcrumbs' => $this->breadcrumblist->display(),
            'message' => $feedback,
        );

        $data = array_merge($form_data, $data);
        $this->template->load('template', 'appsetting/missionportalpref', $data);
    }

    /**
     * updates the setting data
     */
    function _updateData()
    {
        $returnData = array();
        $returnData['status'] = false;
        $returnData['message'] = 'Data update failed';

        $userDirectory = $this->input->post('user_directory') ? $this->input->post('user_directory') : array();
        $blueHostHorizon = set_value('bluehost_threshold_global') * 60;


        $data = array();
        $data['rbac'] = (bool)set_value('rbac');
        $data['authMode'] = set_value('mode');
        $data['ldapEncryption'] = set_value('encryption');
        $data['ldapLoginAttribute'] = set_value('login_attribute');
        $data['ldapHost'] = set_value('host');
        $data["ldapBaseDN"] = set_value('base_dn');
        $data['ldapUsersDirectory'] = implode(';', $userDirectory);
        $data['activeDirectoryDomain'] = set_value('active_directory_domain');
        $data['blueHostHorizon'] = $blueHostHorizon; // store it in seconds

        try
        {
            $this->settings_rest_model->updateData($data);
            $returnData['status'] = true;
            $returnData['message'] = 'Data updated sucessfully';
            return $returnData;
        }
        catch (Exception $e)
        {
            $returnData['status'] = false;
            $returnData['message'] = "Update failed: ".$e->getMessage();
            return $returnData;
        }
        return $returnData;
    }



    function __ldap_check()
    {

        if ($this->input->post('mode') && strtolower($this->input->post('mode')) == 'ldap')
        {
            return '|required';
        }
        else
        {
            return '';
        }
    }

    function __AD_check()
    {

        if ($this->input->post('mode') && strtolower($this->input->post('mode')) == 'active_directory')
        {
            return '|required';
        }
        else
        {
            return '';
        }
    }

    function required_valid_role($value)
    {
        if ($value == "select")
        {
            $this->form_validation->set_message('required_valid_role', 'Required a valid role');
            return false;
        }
        else
        {
            return true;
        }
    }

    function validate_bluehost_threshold($value)
    {
        $value = trim($value);
        if (empty($value))
        {
            $this->form_validation->set_message('validate_bluehost_threshold', 'Unreachable host threshold filed is required.');
            return false;
        }

        if (!ctype_digit($value))
        {
            $this->form_validation->set_message('validate_bluehost_threshold', 'Unreachable host threshold filed must contain a valid number of minutes.');
            return false;
        }

        if ($value < 5 || $value > 50400)
        {
            $this->form_validation->set_message('validate_bluehost_threshold', 'Unreachable host threshold filed must be in range of 5 minutes - 5 weeks (50400 minutes).');
            return false;
        }

        return true;
    }

    function __db_check()
    {
        if ($this->input->post('mode') && strtolower($this->input->post('mode')) == 'internal')
        {
            return '|required';
        }
        else
        {
            return '';
        }
    }

    function logintest()
    {
        $this->load->library('Auth_Ldap');
        $this->auth_ldap->set_host($this->input->post('host'));
        $this->auth_ldap->set_basedn($this->input->post('basedn'));
        $this->auth_ldap->set_mode($this->input->post('mode'));
        if ($this->auth_ldap->get_mode() == 'active_directory' && $this->input->post('login_attr') == '')
        {
            $this->auth_ldap->set_login_attr('sAMAccountName');
        }
        else
        {
            $this->auth_ldap->set_login_attr($this->input->post('login_attr'));
        }
        $this->auth_ldap->set_user_dir($this->input->post('user_dir'));
        //$this->auth_ldap->set_member_attr($this->input->post('member_attr'));
        $this->auth_ldap->set_ad_domain($this->input->post('addomain'));
        $this->auth_ldap->set_encryption($this->input->post('encryption'));

        $result = $this->auth_ldap->login($this->input->post('username'), $this->input->post('password'));
        //previously used for obtaining roles form ldap now local roles is used with ldap users

        $message = "<div>";
        if ($result)
        {
            $message.=" <p class=\"success\">" . $this->lang->line('successful_bind') . "</p>";
        }

        $this->auth_ldap->set_error_delimiters('<p class="error">', '</p>');
        $message.=$this->auth_ldap->errors();
        $message.=$this->auth_ldap->warnings();

        $message.="</div>";
        //echo $message;
        //$ret=array('message'=>$message,'roles'=>form_dropdown('admin_role',$rolesarry));
        $ret = array('message' => $message);
        echo json_encode($ret);
    }

    function ldaptest()
    {
        $data = array();
        foreach ($_POST as $key => $value)
        {
            $data['configsettings'][$key] = $this->input->post($key);
        }
        $this->load->view('appsetting/ldaplogintest', $data);
    }

    function get_native_roles()
    {
        $roles = $this->ion_auth->get_roles();
        $rolesarr = array();
        foreach ($roles as $role)
        {
            $rolesarr[$role['name']] = $role['name'];
        }
        echo form_dropdown('admin_role', $rolesarr);
    }

    function preferences($op = false, $id = false)
    {
        $bc = array(
            'title' => 'preferences',
            'url' => 'settings/preferences',
            'isRoot' => false,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $user = $this->session->userdata('username');
        $this->form_validation->set_rules('tooltips', 'Enable tool tips', 'xss_clean|trim|required');
        $this->form_validation->set_rules('num_rows', 'Number of rows shown in tables', 'xss_clean|trim|required|numeric|integer');
        $this->form_validation->set_error_delimiters('<br /><span>', '</span>');

        if ($this->form_validation->run() == FALSE)
        { // validation hasn't been passed
            $data = array(
                'title' => "CFEngine Mission Portal - Preferences",
                'breadcrumbs' => $this->breadcrumblist->display(),
                'message' => validation_errors(),
                'op' => 'create'
            );
            $settings = $this->settings_model->get_user_settings($user);
            if (is_object($settings))
            {// the information has therefore been successfully saved in the db
                foreach ($settings as $property => $value)
                {
                    if ($property == 'tooltips')
                    {
                        $data[$value] = ' checked="checked"';
                        continue;
                    }
                    if ($property != '_id')
                    {
                        $data[$property] = $this->form_validation->set_value($property, $value);
                    }
                }
                $data['op'] = 'edit';
            }
            else
            {
                $form_data = array(
                    'tooltips' => set_value('tooltips'),
                    'num_rows' => set_value('num_rows', 50)
                );
                $data = array_merge($form_data, $data);
            }
            $this->template->load('template', 'appsetting/userpref', $data);
        }
        else
        {
            $form_data = array(
                'tooltips' => set_value('tooltips'),
                'num_rows' => set_value('num_rows', 50),
                'username' => $user
            );
            // run insert model to write data to db
            $inserted = '';
            if ($op == 'edit')
            {
                $settings = $this->settings_model->get_user_settings($user);
                $inserted = $this->settings_model->update_user_settings($form_data, $settings->_id->__toString());
            }
            else
            {
                $inserted = $this->settings_model->insert_user_settings($form_data);
            }
            if ($inserted)
            {
                $data = array(
                    'title' => "CFEngine Mission Portal - Preferences",
                    'breadcrumbs' => $this->breadcrumblist->display(),
                    'op' => 'edit',
                    'message' => '<p class="success"> Preferences Saved </p>'
                );
                foreach ($form_data as $property => $value)
                {
                    if ($property == 'tooltips')
                    {
                        $data[$value] = ' checked="checked"';
                        continue;
                    }
                    if ($property != '_id')
                    {
                        $data[$property] = $this->form_validation->set_value($property, $value);
                    }
                }
                $this->template->load('template', 'appsetting/userpref', $data);
            }
            else
            {
                echo 'An error occurred saving your information. Please try again later';
            }
        }
    }

}

?>
