<?php

class Hubstatus extends Cf_Controller
{

    function __construct()
    {
        parent::__construct();
    }

    function status()
    {

        $this->load->helper(array('form', 'url'));
        $this->load->library('form_validation');

        $bc = array(
            'title' => $this->lang->line('hub_status'),
            'url' => '/hubstatus/status',
            'isRoot' => false,
            'replace_existing' => true
        );

        $requiredjs = array(
            array('widgets/notes.js')
        );
        $this->carabiner->js($requiredjs);

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('hub_status'),
            'breadcrumbs' => $this->breadcrumblist->display()
        );



        $statusData = cfpr_replica_status();
        $arrayData = json_decode(utf8_encode($statusData), true);

        $data['replicationStatusData'] = $arrayData;
        $this->template->load('template', '/hubstatus/status', $data);
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
            $id = $this->repository_model->approve_policies($username, $repo, $rev, $comments);
            $rev_table = $this->repository_model->get_all_approved_policies($repo, 10);
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
            'title' => 'Approved Policies',
            'url' => '/repository/approvedPolicies',
            'isRoot' => false
        );
        $data = array(
            'title' => "CFEngine Mission Portal - Approved Policies",
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
