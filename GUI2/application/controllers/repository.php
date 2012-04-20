<?php

class Repository extends Cf_Controller
{

    var $username = '';

    function __construct()
    {
        parent::__construct();
        $this->load->library('jcryption');
        $this->load->model('repository_model');
        $this->load->model('settings_model');
        $this->load->helper('form');
        $this->username = $this->session->userdata('username');
        $this->env = ENVIRONMENT;
    }

    function edit()
    {

        $submit = $this->input->post('submit');
        if ($submit == 'Delete')
        {
            $username = $this->session->userdata('username');
            $repoPath = $this->input->post('repoPath');
            $info = array('userId' => $username, 'repoPath' => $repoPath);
            $result = $this->repository_model->delete_repository($info);
            redirect(site_url() . '/repository/manageRepository');
            exit;
        }
    }

    function get_keys()
    {
        $keyLength = 256;
        $keys = $this->jcryption->generateKeypair($keyLength);
        $_SESSION["e"] = array("int" => $keys["e"], "hex" => $this->jcryption->dec2string($keys["e"], 16));
        $_SESSION["d"] = array("int" => $keys["d"], "hex" => $this->jcryption->dec2string($keys["d"], 16));
        $_SESSION["n"] = array("int" => $keys["n"], "hex" => $this->jcryption->dec2string($keys["n"], 16));

        echo '{"e":"' . $_SESSION["e"]["hex"] . '","n":"' . $_SESSION["n"]["hex"] . '","maxdigits":"' . intval($keyLength * 2 / 16 + 3) . '"}';
    }

    function checkOutUrl($url = '')
    {

        $url = $url ? urldecode($url) : $this->input->post('repourl');
        $currentUser = $this->session->userdata('username');
        $obj = $this->repository_model->get_specific_repository($currentUser, $url);

        if ($obj != NULL)
        {
            // var_dump($obj);
            $info = array('userId' => $obj->userId, 'password' => $obj->password);
            $username = $obj->username != '' ? $obj->username : NULL;
            $password = NULL;
            if ($info['password'] != '')
            {
                if ($this->settings_model->app_settings_get_item('mode') != 'database')
                {
                    $password = $this->repository_model->decrypt_password($info, $this->session->userdata('pwd'));
                }
                else
                {
                    $password = $this->repository_model->decrypt_password($info);
                }
            }
            $params = array(
                'username' => $username,
                'password' => $password,
                'repository' => $url,
                'workingdir' => get_policiesdir() . $this->session->userdata('username')
            );



            $errorArray = array();
            try
            {
                $this->load->library('cfsvn', $params);
                $data = $this->cfsvn->cfsvn_checkout();
            }
            catch (Exception $e)
            {
                $errorArray = array('message' => $e->getMessage());
            }

            if (!empty($errorArray))
            {

                $bc = array(
                    'title' => $this->lang->line('breadcrumb_repo_manage'),
                    'url' => site_url() . '/repository/manageRepository',
                    'isRoot' => false,
                    'replace_existing' => true
                );

                $this->breadcrumb->setBreadCrumb($bc);

                $data = array(
                    'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_repo_manage'),
                    'breadcrumbs' => $this->breadcrumblist->display()
                );
                $data['url'] = $url;
                $data['errorArray'] = $errorArray;
                $data['env'] = $this->env;

                $this->template->load('template', '/repository/check_out_error', $data);
            }
            else
            {
                //check out successful
                redirect(site_url() . '/cfeditor');
                exit;
            }
        }
    }

    function checkOut($force = '')
    {
        $this->load->helper(array('form', 'url', 'svn'));
        $this->load->library('form_validation');
        $data = array();

        $params = array(
            'workingdir' => get_policiesdir() . $this->session->userdata('username')
        );

        $currentUser = $this->session->userdata('username');

        try
        {
            $return = $this->load->library('cfsvn', $params);
            $url = $this->cfsvn->get_current_repository();
            $obj = $this->repository_model->get_specific_repository($currentUser, $url);
        }
        catch (Exception $e)
        {
            // if we get errors from svn status
            $obj = null;
        }
        $alreadyCheckedOut = is_svn_checked_out('./policies/', $this->session->userdata('username'));

        // check if it is already checked out  checked out
        if ($alreadyCheckedOut && !$force && $obj != NULL)
        {
            $hasChanges = array();
            try
            {
                $hasChanges = $this->cfsvn->cfsvn_working_copy_status();
            }
            catch (Exception $e)
            {
                log_message('', $e->__toString());
            }
            $data['hasChanges'] = $hasChanges;
            $this->load->view('/repository/already_checked_out', $data);
            return;
        }

        if ($this->input->post('addnew'))
        {
            $this->form_validation->set_error_delimiters('<span class="error">', '</span>');
            $this->form_validation->set_rules('repoPath', 'Repository path', 'required');
            if ($this->form_validation->run())
            {

                $info = array('userId' => $this->session->userdata('username'),
                    'repoPath' => $this->input->post('repoPath'),
                    'username' => $this->input->post('username'),
                    'password' => $this->input->post('password'));
                $return = $this->_insert_repository($info);

                if (!$return)
                {
                    $this->form_validation->_error_array = array_merge($this->form_validation->_error_array, $this->repository_model->get_errors());
                }
                else
                {
                    $successMessage = sprintf('%s. <a href="%s/repository/checkOutUrl/%s" target="_self">%s</a>', $this->lang->line('repo_add_sucess'), site_url(), urlencode($this->input->post('repoPath')), $this->lang->line('proceed_to_editor'));
                    $this->session->set_flashdata(array('success' => $successMessage));
                    redirect(current_url());
                    exit();
                }
            }
        }
        $repo = $this->repository_model->get_all_repository($this->username);
        $data['repoData'] = $repo;
        $data['errors'] = $this->form_validation->_error_array;
        $data['addFormPath'] = site_url() . '/repository/checkout/force';

        $this->load->view('/repository/checkout_repository', $data);
    }

    function addSuccess()
    {

        $this->loadView('/repository/addSuccess');
    }

    function _insert_repository($info)
    {

        //password is jencrypted before so decrypt to clear one .
        if ($info['password'] != '')
        {
            $info['password'] = $this->jcryption->decrypt($info['password'], $_SESSION["d"]["int"], $_SESSION["n"]["int"]);
            // var_dump($info);
        }
        if ($this->settings_model->app_settings_get_item('mode') != 'database')
        {
            $return = $this->repository_model->insert_repository($info, $this->session->userdata('pwd'));
        }
        else
        {
            $return = $this->repository_model->insert_repository($info);
        }
        return $return;
    }

    function manageRepository()
    {

        $this->load->helper(array('form', 'url'));
        $this->load->library('form_validation');

        $bc = array(
            'title' => $this->lang->line('breadcrumb_repo_manage'),
            'url' => '/repository/manageRepository',
            'isRoot' => false,
            'replace_existing' => true
        );

        $requiredjs = array(
            array('widgets/notes.js'),
            array('jquery.jcryption-1.1.min.js')
        );
        $this->carabiner->js($requiredjs);

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_repo_manage'),
            'title_header' => "Repository overview",
            'breadcrumbs' => $this->breadcrumblist->display()
        );


        if ($this->input->post('addnew'))
        {
            $this->form_validation->set_error_delimiters('<span class="error">', '</span>');
            $this->form_validation->set_rules('repoPath', 'Repository path', 'required');
            //$this->form_validation->set_rules('username', 'Username', 'required');
            if ($this->form_validation->run())
            {
                $info = array('userId' => $this->session->userdata('username'),
                    'repoPath' => $this->input->post('repoPath'),
                    'username' => $this->input->post('username'),
                    'password' => $this->input->post('password'));
                $return = $this->_insert_repository($info);
                if (!$return)
                {
                    $this->form_validation->_error_array = array_merge($this->form_validation->_error_array, $this->repository_model->get_errors());
                }
                else
                {
                    $this->session->set_flashdata(array('success' => 'Repository added sucessfully'));
                    redirect(current_url());
                }
            }
        }
        $repo = $this->repository_model->get_all_repository($this->username);
        $data['repoData'] = $repo;
        $data['errors'] = $this->form_validation->_error_array;
        $this->template->load('template', '/repository/manage_repository', $data);
    }

    function policyApprover()
    {
        $repos = $this->repository_model->get_all_repository($this->username);
        if (count($repos) > 0)
        {
            $repo_url = array();
            foreach ($repos as $repo)
            {
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
        }
        else
        {
            echo "<span class=\"info maxwidth400\">" . $this->lang->line('no_repos') . " Use <strong class=\"underline\">" .
            anchor('repository/manageRepository/', 'Manage Repository', 'target=_self') . '</strong> to add a repository</span>.';
        }
    }

    function getListofRev()
    {
        $repo = $this->input->post('repo');
        $revs = $this->repository_model->get_revisions($repo);
        if (count($revs) > 0)
        {
            echo form_dropdown('rev', array_combine($revs, $revs), $revs[0]);
        }
        else
        {
            echo " ";
        }
    }

    function approveRepoAction()
    {
        $this->load->library('table');
        $username = $this->session->userdata('username');
        $repo = $this->input->post('repository');
        $rev = $this->input->post('revision');
        $comments = $this->input->post('comments');
        if (!$rev)
        {
            echo "<span class=\"info maxwidth400\">" . $this->lang->line('no_revisions') . "</span>";
            return;
        }
        try
        {
            $is_duplicate = $this->repository_model->check_duplicate_approval($repo, $rev, $username);
            if ($is_duplicate)
            {
                echo "<span class=\"error maxwidth400\">" . $this->lang->line('already_approved') . "</span> <br />" .
                anchor('/repository/approvedPolicies', 'Approved log', array('target' => "_self"));
                ;
                return;
            }
            $id = $this->repository_model->approve_policies($username, $repo, $rev, $comments);
            $rev_table = $this->repository_model->get_all_approved_policies(array($repo), 10);
            $data = array('table' => $rev_table);
        }
        catch (Exception $e)
        {
            $data = array("error" => $e->getMessage());
        }
        $this->load->view('repository/approved_policies', $data);
    }

    function approvedPolicies()
    {
        $this->load->library('table');
        $repos = $this->repository_model->get_all_repository($this->session->userdata('username'));
        foreach ($repos as $repo)
        {
            $reposlist[] = $repo['repoPath'];
        }
        $params = $this->uri->uri_to_assoc(3);
        $rows = isset($params['rows']) ? $params['rows'] : ($this->input->post('rows') ? $this->input->post('rows') : $this->setting_lib->get_no_of_rows());
        if (is_numeric($rows))
        {
            $rows = (int) $rows;
        }
        else
        {
            $rows = 20;
        }

        $page = isset($params['page']) ? intval($params['page'], 10) : 1;
        $total = $this->repository_model->count_all_approved_policies($reposlist);
        $skip = (int) ($rows * ($page - 1));
        $rev_table = $this->repository_model->get_all_approved_policies($reposlist, $rows, $skip);

        //$rev_table = $this->repository_model->get_all_approved_policies();
        $bc = array(
            'title' => $this->lang->line('breadcrumb_approved_policies'),
            'url' => '/repository/approvedPolicies',
            'isRoot' => false
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_approved_policies'),
            'title_header' => "policies approved",
            'table' => $rev_table,
            'breadcrumbs' => $this->breadcrumblist->display(),
            'rows_per_page' => $rows,
            'current_page' => $page,
            'total' => $total,
        );
        $this->template->load('template', 'repository/approved_policies_fullview', $data);
    }

}

?>
