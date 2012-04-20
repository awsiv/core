<?php

class Promise extends Cf_Controller
{

    function Promise()
    {
        parent::__construct();
        $this->load->model(array('promise_model', 'knowledge_model'));
    }

    function index()
    {
        $this->details();
    }

    function details($handle = NULL)
    {
        $this->carabiner->css('tabs-custom.css');

        $username = $this->session->userdata('username');

        if (is_null($handle))
        {
            $handle = isset($_POST['handle']) ? $_POST['handle'] : NULL;
        }
        else
        {
            $handle = urldecode($handle);
        }
        $bc = array(
            'title' => $this->lang->line('breadcrumb_promise'),
            'url' => 'promise/details/' . $handle,
            'isRoot' => false,
            'replace_existing' => true
        );


        $this->breadcrumb->setBreadCrumb($bc);


        $tmp_arr = array();
        $allhandlespromiser = array();
        $allhandlesbytype = array();
        $allhandles = array();

        $topicDetail = array();
        $topicLeads = array();
        try
        {

            $mybundle = $this->promise_model->getBundleByPromiseHandle($username, $handle);


            // get promise details
            $promise = $this->promise_model->getPromiseDetails($username, $handle);
            $promiser = key_exists('promiser', $promise) ? $promise['promiser'] : "";
            $type = key_exists('promise_type', $promise) ? $promise['promise_type'] : "";


            //$pid = cfpr_get_pid_for_topic($username,"promises", $handle);
            $pid = $this->knowledge_model->getPidForTopic($username, "promises", $handle);
            //$topicDetail = cfpr_show_topic($username,$pid);
            if ($pid != 0)
            {
                $topicDetail = $this->knowledge_model->showTopics($username, $pid);
                $topicLeads = $this->knowledge_model->showTopicLeadsWithName($username, $handle);
            }

            $tmp_arr = $this->promise_model->getPromiseListByPromiser($username, $promiser);
            foreach ((array) $tmp_arr['data'] as $item => $value)
            {
                $allhandlespromiser[] = $value[0];
            }

            $tmp_arr = $this->promise_model->getPromiseListByType($username, $type);
            foreach ((array) $tmp_arr['data'] as $item => $value)
            {
                $allhandlesbytype[] = $value[0];
            }


            $tmp_arr = $this->promise_model->getPromiseListByBundle($username, $mybundle);
            foreach ((array) $tmp_arr['data'] as $item => $value)
            {
                $allhandles[] = $value[0];
            }


            $data = array(
                'handle' => $handle,
                'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_promise') . " " . $handle,
                'pid' => $pid,
                'type' => $type,
                'promise' => $promise,
                'allhandlespromiser' => $allhandlespromiser,
                'allhandlesbytype' => $allhandlesbytype,
                'allhandles' => $allhandles,
                'mybundle' => $mybundle,
                'topicLeads' => $topicLeads,
                'topicDetail' => $topicDetail,
                'breadcrumbs' => $this->breadcrumblist->display()
            );
            if (is_ajax())
            {
                $this->load->view('promise/promise', $data);
            }
            else
            {
                $this->template->load('template', 'promise/promise', $data);
            }
        }
        catch (Exception $e)
        {
            if ($e->getCode() == 204)
            {
                $data = array(
                    'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_promise') . " " . $handle,
                    'breadcrumbs' => $this->breadcrumblist->display(),
                    'error' => $e->getMessage()
                );
                $this->template->load('template', 'promise/promise', $data);
            }
            else
            {
                show_error($e->getMessage(), 500);
            }
        }
    }

}
