<?php

class Settings extends Cf_Controller {

    function __construct() {
        parent::__construct();
        $this->load->library('form_validation');
        $this->load->library('ion_auth');
        $this->load->helper('form');
        $this->load->helper('url');
        $this->load->model('settings_model');
    }

    function manage($op=false, $id=false) {
        $bc = array(
            'title' => 'settings',
            'url' => 'auth/setting',
            'isRoot' => false,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);

        if (!$this->ion_auth->is_accessable()) {
            redirect('auth/setting');
        }
        $requiredjs = array(
            array('widgets/notes.js'),
        );
        $this->carabiner->js($requiredjs);
        $required_if_ldap = $this->__ldap_check();
        $required_if_database = $this->__db_check();
        $required_if_ad = $this->__AD_check();
        $this->form_validation->set_rules('appemail', 'Administrative email', 'xss_clean|trim|required|valid_email');
        $this->form_validation->set_rules('mode', 'Authentication mode', 'xss_clean|trim|required');
        $this->form_validation->set_rules('host', 'host', 'xss_clean|trim' . $required_if_ldap . $required_if_ad);
        $this->form_validation->set_rules('base_dn', 'base dn', 'xss_clean|trim' . $required_if_ldap . $required_if_ad);
        $this->form_validation->set_rules('login_attribute', 'login attribute', 'xss_clean|trim' . $required_if_ldap);
        $this->form_validation->set_rules('users_directory[]', 'users directory', 'xss_clean' . $required_if_ldap);
        $this->form_validation->set_rules('active_directory_domain', 'active directory domain', 'xss_clean|trim' . $required_if_ad);
        $this->form_validation->set_rules('member_attribute', 'memberof attribute', 'xss_clean|trim');
        $this->form_validation->set_rules('fall_back_for', 'valid group', 'callback_required_valid_group');
        $this->form_validation->set_rules('admin_group', 'valid group');
        $this->form_validation->set_rules('encryption', 'Encryption mode', 'xss_clean|trim' . $required_if_ldap . $required_if_ad);
        $this->form_validation->set_error_delimiters('<span>', '</span><br/>');

        if ($this->form_validation->run() == FALSE) { // validation hasn't been passed
            $data = array(
                'title' => "CFEngine Mission Portal - Settings",
                'breadcrumbs' => $this->breadcrumblist->display(),
                'message' => validation_errors(),
                'op' => 'create'
            );

            //for fall back groups list
            $groups = $this->ion_auth->get_groups_fromdb();
            foreach ((array) $groups as $group) {
                $data['groups'][$group['name']] = $group['name'];
            }
            $data['groups']['select'] = "--select-one--";

            //for selecting admin_group from list, populated list depends on the mode selected and saved
            $groups_acc_mode = $this->ion_auth->get_groups();
            foreach ((array) $groups_acc_mode as $group) {
                key_exists('name', $group) ? $data['groupsacc'][$group['name']] = $group['name'] : $data['groupsacc'][$group['displayname']] = $group['displayname'];
            }
            $data['groupsacc']['select'] = "--select-one--";
            //if previous settings exist load it and display
            $settings = $this->settings_model->get_app_settings();

            if (is_object($settings)) {// the information has therefore been successfully saved in the db
                foreach ($settings as $property => $value) {
                    if ($property == 'mode' || $property == 'active_directory' || $property == 'encryption') {
                        if (!$this->input->post('mode')) {
                            $data[$value] = 'checked="checked"';
                            continue;
                        }
                    }
                    if ($property == 'users_directory') {
                        $user_dir = explode(';', $value);
                        $i = 0;
                        foreach ($user_dir as $dirs) {
                            if ($i > 0) {
                                $data['user_dirs'][$i] = array('name' => 'users_directory[]',
                                    'id' => 'users_directory_' . $i,
                                    'type' => 'text',
                                    'value' => $this->form_validation->set_value('users_directory[]', $dirs),
                                );
                            } else {
                                $data[$property] = $this->form_validation->set_value('users_directory[]', $user_dir[$i]);
                            }
                          $i++;
                        }
                        continue;
                    }
                    if ($property != '_id') {
                        $data[$property] = $this->form_validation->set_value($property, $value);
                    }
                }
                $data['op'] = 'edit';
            } else {
                $form_data = array(
                    'appemail' => set_value('appemail'),
                    'mode' => set_value('mode'),
                    'host' => set_value('host'),
                    'base_dn' => set_value('base_dn'),
                    'login_attribute' => set_value('login_attribute'),
                    //'users_directory[]' => set_value('users_directory[]'),
                    //'active_directory' => set_value('active_directory'),
                    'active_directory_domain' => set_value('active_directory_domain'),
                    'member_attribute' => set_value('member_attribute'),
                    'encryption' => set_value('encryption'),
                );
                
                
                $data = array_merge($form_data, $data);
            }
            $this->template->load('template', 'appsetting/missionportalpref', $data);
        } else {
            $user_dir =$this->input->post('users_directory');
            $form_data = array(
                'appemail' => set_value('appemail'),
                'mode' => set_value('mode'),
                'host' => set_value('host'),
                'base_dn' => set_value('base_dn'),
                'login_attribute' => set_value('login_attribute'),
                'users_directory' => implode(';', $this->input->post('users_directory')),
                //  	'active_directory' => set_value('active_directory'),
                'active_directory_domain' => set_value('active_directory_domain'),
                'member_attribute' => set_value('member_attribute'),
                'fall_back_for' => $this->input->post('fall_back_for'),
                'admin_group' => $this->input->post('admin_group'),
                'encryption' => set_value('encryption')
            );

// run insert model to write data to db
            $inserted = '';
            if ($op == 'edit') {
                $settings = $this->settings_model->get_app_settings();
                $inserted = $this->settings_model->update_app_settings($form_data, $settings->_id->__toString());
                $this->ion_auth->set_mode($form_data['mode']);
            } else {
                $inserted = $this->settings_model->insert_app_settings($form_data);
            }
            if ($inserted) {// the information has therefore been successfully saved in the db
//redirect('settings/success');   
                $data = array(
                    'title' => "CFEngine Mission Portal - Settings",
                    'breadcrumbs' => $this->breadcrumblist->display(),
                    'op' => 'edit',
                    'message' => '<p class="success"> Settings configured sucessfully</p>'
                );
                foreach ($form_data as $property => $value) {
                    if ($property == 'mode' || $property == 'encryption') {
                        $data[$value] = ' checked="checked"';
                        continue;
                    }
                     if ($property == 'users_directory' && is_array($user_dir)) {
                        $i = 0;
                        foreach ($user_dir as $dirs) {
                            if ($i > 0) {
                                $data['user_dirs'][$i] = array('name' => 'users_directory[]',
                                    'id' => 'users_directory_' . $i,
                                    'type' => 'text',
                                    'value' => $dirs,
                                );
                            }else{
                                $data[$property] = $this->form_validation->set_value('users_directory[]', $user_dir[$i]);
                            }
                            $i++;
                        }
                        continue;
                    }
                    if ($property != '_id') {
                        $data[$property] = $this->form_validation->set_value($property, $value);
                    }
                }
//$this->load->view('appsetting/missionportalpref',$data);
                $groups = $this->ion_auth->get_groups_fromdb();
                foreach ((array) $groups as $group) {
                    $data['groups'][$group['name']] = $group['name'];
                }
                $data['groups']['select'] = "--select-one--";
                $groups_acc_mode = $this->ion_auth->get_groups();
                foreach ((array) $groups_acc_mode as $group) {
                    key_exists('name', $group) ? $data['groupsacc'][$group['name']] = $group['name'] : $data['groupsacc'][$group['displayname']] = $group['displayname'];
                }
                $data['groupsacc']['select'] = "--select-one--";
                $this->template->load('template', 'appsetting/missionportalpref', $data);
            } else {
                echo 'An error occurred saving your information. Please try again later';
// Or whatever error handling is necessary
            }
        }
    }

    function __ldap_check() {

        if ($this->input->post('mode') && strtolower($this->input->post('mode')) == 'ldap') {
            return '|required';
        } else {
            return '';
        }
    }

    function __AD_check() {

        if ($this->input->post('mode') && strtolower($this->input->post('mode')) == 'active_directory') {
            return '|required';
        } else {
            return '';
        }
    }

    function required_valid_group($value) {
        if ($value == "select") {
            $this->form_validation->set_message('required_valid_group', 'Required a valid group');
            return false;
        } else {
            return true;
        }
    }

    function __db_check() {
        if ($this->input->post('mode') && strtolower($this->input->post('mode')) == 'database') {
            return '|required';
        } else {
            return '';
        }
    }

    function logintest() {
        $this->load->library('Auth_Ldap');
        $this->auth_ldap->set_host($this->input->post('host'));
        $this->auth_ldap->set_basedn($this->input->post('basedn'));
        $this->auth_ldap->set_mode($this->input->post('mode'));
        if ($this->auth_ldap->get_mode() == 'active_directory' && $this->input->post('login_attr') == '') {
            $this->auth_ldap->set_login_attr('sAMAccountName');
        } else {
            $this->auth_ldap->set_login_attr($this->input->post('login_attr'));
        }
        $this->auth_ldap->set_user_dir($this->input->post('user_dir'));
        $this->auth_ldap->set_member_attr($this->input->post('member_attr'));
        $this->auth_ldap->set_ad_domain($this->input->post('addomain'));
        $this->auth_ldap->set_encryption($this->input->post('encryption'));

        $result = $this->auth_ldap->login($this->input->post('username'), $this->input->post('password'));
//print_r($result);
        $message = "<div id=\"infoMessage\" style=\"margin-top:20px\">";
        if ($result) {
            $message.=" <p class=\"success\">" . $this->lang->line('successful_bind') . "</p>";
        }

        $this->auth_ldap->set_error_delimiters('<p class="error">', '</p>');
        $message.=$this->auth_ldap->errors();
        $message.=$this->auth_ldap->warnings();

        $message.="</div>";
        echo $message;
    }

    function ldaptest() {
        $data = array();
        foreach ($_POST as $key => $value) {
            $data['configsettings'][$key] = $this->input->post($key);
        }
        $this->load->view('appsetting/ldaplogintest', $data);
    }

    function preferences($op=false, $id=false) {
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

        if ($this->form_validation->run() == FALSE) { // validation hasn't been passed
            $data = array(
                'title' => "CFEngine Mission Portal - Preferences",
                'breadcrumbs' => $this->breadcrumblist->display(),
                'message' => validation_errors(),
                'op' => 'create'
            );
            $settings = $this->settings_model->get_user_settings($user);
            if (is_object($settings)) {// the information has therefore been successfully saved in the db
                foreach ($settings as $property => $value) {
                    if ($property == 'tooltips') {
                        $data[$value] = ' checked="checked"';
                        continue;
                    }
                    if ($property != '_id') {
                        $data[$property] = $this->form_validation->set_value($property, $value);
                    }
                }
                $data['op'] = 'edit';
            } else {
                $form_data = array(
                    'tooltips' => set_value('tooltips'),
                    'num_rows' => set_value('num_rows', 50)
                );
                $data = array_merge($form_data, $data);
            }
            $this->template->load('template', 'appsetting/userpref', $data);
        } else {
            $form_data = array(
                'tooltips' => set_value('tooltips'),
                'num_rows' => set_value('num_rows', 50),
                'username' => $user
            );
            // run insert model to write data to db
            $inserted = '';
            if ($op == 'edit') {
                $settings = $this->settings_model->get_user_settings($user);
                $inserted = $this->settings_model->update_user_settings($form_data, $settings->_id->__toString());
            } else {
                $inserted = $this->settings_model->insert_user_settings($form_data);
            }
            if ($inserted) {
                $data = array(
                    'title' => "CFEngine Mission Portal - Preferences",
                    'breadcrumbs' => $this->breadcrumblist->display(),
                    'op' => 'edit',
                    'message' => '<p class="success"> Preferences Saved </p>'
                );
                foreach ($form_data as $property => $value) {
                    if ($property == 'tooltips') {
                        $data[$value] = ' checked="checked"';
                        continue;
                    }
                    if ($property != '_id') {
                        $data[$property] = $this->form_validation->set_value($property, $value);
                    }
                }
                $this->template->load('template', 'appsetting/userpref', $data);
            } else {
                echo 'An error occurred saving your information. Please try again later';
            }
        }
    }

}

?>
