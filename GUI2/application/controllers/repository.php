<?php

class Repository extends Cf_Controller {

    var $username = '';

    function __construct() {
        parent::__construct();
        $this->load->model('repository_model');
        $this->username = $this->session->userdata('username');
    }

    function insertRepository() {
        $info = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2', 'username' => 'something', 'password' => 'password');
        $return = $this->repository_model->insert_repository($info);
        if (!$return) {
            var_dump($this->repository_model->get_errors());
        } else {
            var_dump($return);
        }
    }

    function getRepository() {
        $repo = $this->repository_model->get_all_repository();
        foreach ($repo as $obj) {
            $info = array('userId' => $obj['userId'], 'password' => $obj['password']);
            $pass = $this->repository_model->decrypt_password($info);
            //var_dump($obj['password']);
            echo "Enc pass :: " . $obj['password'] . " original pass :: " . $pass;
            echo "<br>";
        }
    }

    function updateRepository() {
        $info = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2', 'username' => 'something', 'password' => 'password');
        $newInfo = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2/new', 'username' => 'newsomething', 'password' => 'newpassword');
        $result = $this->repository_model->update_repository($info, $newInfo);
        if (!$result) {
            var_dump($this->repository_model->get_errors());
        }
    }

    function deleteRepository() {

        $info = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2', 'username' => 'something', 'password' => 'password');

        $result = $this->repository_model->delete_repository();
        if (!$result) {
            var_dump($this->repository_model->get_errors());
        }
    }

    function edit() {

        $submit = $this->input->post('submit');
        if ($submit == 'Delete') {
            $username = $this->session->userdata('username');
            $repoPath = $this->input->post('repoPath');
            $info = array('userId' => $username, 'repoPath' => $repoPath);
            $result = $this->repository_model->delete_repository($info);
            redirect('/repository/manageRepository');
            exit;
        }
    }

    function checkOutUrl($url='') {

        $url = $url ? $url : $this->input->post('repourl');
        $currentUser = $this->session->userdata('username');
        $obj = $this->repository_model->get_specific_repository($currentUser, $url);



        if ($obj != NULL) {

            $info = array('userId' => $obj->userId, 'password' => $obj->password);
            $username = $obj->username;

            $password = $this->repository_model->decrypt_password($info);


            $params = array(
                'username' => $username,
                'password' => $password,
                'repository' => $url,
                'workingdir' => get_policiesdir() . $this->session->userdata('username')
            );

            $this->load->library('cfsvn', $params);
            $errorArray = array();
            try {
                $data = $this->cfsvn->cfsvn_checkout();
            } catch (Exception $e) {
                $errorArray = array('message' => $e->getMessage());
            }

            if (!empty($errorArray)) {

                $bc = array(
                    'title' => 'Manage Repository',
                    'url' => 'repository/manageRepository',
                    'isRoot' => false
                );

                $this->breadcrumb->setBreadCrumb($bc);

                $data = array(
                    'title' => "Cfengine Mission Portal - overview",
                    'title_header' => "Repository overview",
                    'breadcrumbs' => $this->breadcrumblist->display()
                );
                $data['url'] = $url;

                $this->template->load('template', '/repository/check_out_error', $data);
            } else {
                //check out successful
                redirect('/cfeditor');
                exit;
            }
        }
    }

    function checkOut($force ='') {
        $this->load->helper(array('form', 'url', 'svn'));
        $this->load->library('form_validation');
        $data = array();

        $alreadyCheckedOut = is_svn_checked_out('./policies/', $this->session->userdata('username'));

        // check if it is already checked out  checked out 
        if ($alreadyCheckedOut && !$force) {
            $this->load->view('/repository/already_checked_out', $data);
            return;
        }

        if ($this->input->post('addnew')) {
            $this->form_validation->set_error_delimiters('<span class="error">', '</span>');
            $this->form_validation->set_rules('repoPath', 'Repository path', 'required');
            $this->form_validation->set_rules('username', 'Username', 'required');
            if ($this->form_validation->run()) {

                $info = array('userId' => $this->session->userdata('username'),
                    'repoPath' => $this->input->post('repoPath'),
                    'username' => $this->input->post('username'),
                    'password' => $this->input->post('password'));
                $return = $this->repository_model->insert_repository($info);

                if (!$return) {
                    $this->form_validation->_error_array = array_merge($this->form_validation->_error_array, $this->repository_model->get_errors());
                } else {
                    $successMessage = sprintf ('Repository added sucessfully. <a href="/repository/checkouturl/%s" target="_self">Go to policy editor</a>',$this->input->post('repoPath'));
                    $this->session->set_flashdata(array('success' =>$successMessage ));
                    redirect(current_url());
                    exit();
                }
            }
        }
        $repo = $this->repository_model->get_all_repository($this->username);
        $data['repoData'] = $repo;
        $data['errors'] = $this->form_validation->_error_array;


        $this->load->view('/repository/checkout_repository', $data);
    }

    function addSuccess() {

        $this->loadView('/repository/addSuccess');
    }

    function manageRepository() {

        $this->load->helper(array('form', 'url'));
        $this->load->library('form_validation');

        $bc = array(
            'title' => 'Manage Repository',
            'url' => 'repository/manageRepository',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => "Cfengine Mission Portal - overview",
            'title_header' => "Repository overview",
            'breadcrumbs' => $this->breadcrumblist->display()
        );


        if ($this->input->post('submit')) {
            $this->form_validation->set_error_delimiters('<span class="error">', '</span>');
            $this->form_validation->set_rules('repoPath', 'Repository path', 'required');
            $this->form_validation->set_rules('username', 'Username', 'required');
            if ($this->form_validation->run()) {
                $info = array('userId' => $this->session->userdata('username'),
                    'repoPath' => $this->input->post('repoPath'),
                    'username' => $this->input->post('username'),
                    'password' => $this->input->post('password'));
                $return = $this->repository_model->insert_repository($info);
                if (!$return) {
                    $this->form_validation->_error_array = array_merge($this->form_validation->_error_array, $this->repository_model->get_errors());
                } else {
                    $this->session->set_flashdata(array('success' => 'Repository added sucessfully'));
                    redirect(current_url());
                    exit();
                }
            }
        }
        $repo = $this->repository_model->get_all_repository($this->username);
        $data['repoData'] = $repo;
        $data['errors'] = $this->form_validation->_error_array;
        $this->template->load('template', '/repository/manage_repository', $data);
    }

}

?>
