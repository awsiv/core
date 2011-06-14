<?php

class Repository extends Cf_Controller {

    var $username = '';

    function __construct() {
        parent::__construct();
        $this->load->library('jcryption');
        $this->load->model('repository_model');
        $this->load->helper('form');
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

    function get_keys() {
        $keyLength = 256;
        $keys = $this->jcryption->generateKeypair($keyLength);
        $_SESSION["e"] = array("int" => $keys["e"], "hex" => $this->jcryption->dec2string($keys["e"], 16));
        $_SESSION["d"] = array("int" => $keys["d"], "hex" => $this->jcryption->dec2string($keys["d"], 16));
        $_SESSION["n"] = array("int" => $keys["n"], "hex" => $this->jcryption->dec2string($keys["n"], 16));

        echo '{"e":"' . $_SESSION["e"]["hex"] . '","n":"' . $_SESSION["n"]["hex"] . '","maxdigits":"' . intval($keyLength * 2 / 16 + 3) . '"}';
    }

    function checkOutUrl($url='') {

        $url = $url ? urldecode($url) : $this->input->post('repourl');
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

            $params = array(
                'workingdir' => get_policiesdir() . $this->session->userdata('username')
            );

            $this->load->library('cfsvn', $params);
            $hasChanges = array();
            try {
                $hasChanges = $this->cfsvn->cfsvn_working_copy_status();
            } catch (Exception $e) {
                //$this->log($e->__toString());
                var_dump($e->__toString());
            }
            $data['hasChanges'] = $hasChanges;
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
                $return = $this->_insert_repository($info);

                if (!$return) {
                    $this->form_validation->_error_array = array_merge($this->form_validation->_error_array, $this->repository_model->get_errors());
                } else {
                    $successMessage = sprintf('Repository added sucessfully. <a href="/repository/checkOutUrl/%s" target="_self">Go to policy editor</a>', urlencode($this->input->post('repoPath')));
                    $this->session->set_flashdata(array('success' => $successMessage));
                    redirect(current_url());
                    exit();
                }
            }
        }
        $repo = $this->repository_model->get_all_repository($this->username);
        $data['repoData'] = $repo;
        $data['errors'] = $this->form_validation->_error_array;
        $data['addFormPath'] = '/repository/checkout/force';

        $this->load->view('/repository/checkout_repository', $data);
    }

    function addSuccess() {

        $this->loadView('/repository/addSuccess');
    }

    function _insert_repository($info) {



        //password is jencrypted before so decrypt to clear one .

        $info['password'] = $this->jcryption->decrypt($info['password'], $_SESSION["d"]["int"], $_SESSION["n"]["int"]);

        $return = $this->repository_model->insert_repository($info);
        return $return;
    }

    function manageRepository() {

        $this->load->helper(array('form', 'url'));
        $this->load->library('form_validation');

        $bc = array(
            'title' => 'Manage Repository',
            'url' => 'repository/manageRepository',
            'isRoot' => false
        );

        $requiredjs = array(
            array('widgets/notes.js'),
            array('jquery.jcryption-1.1.min.js')
        );
        $this->carabiner->js($requiredjs);

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => "Cfengine Mission Portal - overview",
            'title_header' => "Repository overview",
            'breadcrumbs' => $this->breadcrumblist->display()
        );


        if ($this->input->post('addnew')) {
            $this->form_validation->set_error_delimiters('<span class="error">', '</span>');
            $this->form_validation->set_rules('repoPath', 'Repository path', 'required');
            $this->form_validation->set_rules('username', 'Username', 'required');
            if ($this->form_validation->run()) {
                $info = array('userId' => $this->session->userdata('username'),
                    'repoPath' => $this->input->post('repoPath'),
                    'username' => $this->input->post('username'),
                    'password' => $this->input->post('password'));
                $return = $this->_insert_repository($info);
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

    function policyApprover() {
        $repos = $this->repository_model->get_all_repository($this->username);
        if (count($repos) > 0) {
            $repo_url = array();
            foreach ($repos as $repo) {
                array_push($repo_url, $repo['repoPath']);
            }
            //by default first option is selected and populate a list of revs for it
            $revs = $this->repository_model->get_revisions($repo_url[0]);
            $data = array(
                'reposoptions' => $repo_url,
                'revs' => $revs
            );
            $requiredjs = array(
                array('jquery.autogrowtextarea.js')
            );
            $this->carabiner->js($requiredjs);
            // $this->template->load('template','repository/approver',$data);
            $this->load->view('repository/approver', $data);
        } else {
            echo "<span class=\"info maxwidth400\">" . $this->lang->line('no_repos') . " Use <strong class=\"underline\">" .
            anchor('repository/manageRepository/', 'Manage Repository', 'target=_self') . '</strong> to add a repository</span> ';
        }
    }

    function getListofRev() {
        $repo = $this->input->post('repo');
        $revs = $this->repository_model->get_revisions($repo);
        if (count($revs) > 0) {
            echo form_dropdown('rev', array_combine($revs, $revs), $revs[0]);
        } else {
            echo " ";
        }
    }

    function approveRepoAction() {
        $this->load->library('table');
        $username = $this->session->userdata('username');
        $repo = $this->input->post('repository');
        $rev = $this->input->post('revision');
        $comments = $this->input->post('comments');
        if (!$rev) {
            echo "<span class=\"info maxwidth400\">" . $this->lang->line('no_revisions') . "</span>";
            return;
        }
        try {
            $id = $this->repository_model->approve_policies($username, $repo, $rev, $comments);
            $rev_table = $this->repository_model->get_all_approved_policies($repo, 10);
            $data = array('table' => $rev_table);
        } catch (Exception $e) {
            $data = array("error" => $e->getMessage());
        }
        $this->load->view('repository/approved_policies', $data);
    }

    function approvedPolicies() {
        $this->load->library('table');
        $rev_table = $this->repository_model->get_all_approved_policies();
        $bc = array(
            'title' => 'Approved Policies',
            'url' => 'repository/approvedPolicies',
            'isRoot' => false
        );
        $data = array(
            'title' => "Cfengine Mission Portal - approved policies",
            'title_header' => "policies approved",
            'table' => $rev_table,
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'repository/approved_policies_fullview', $data);
    }

}

?>
