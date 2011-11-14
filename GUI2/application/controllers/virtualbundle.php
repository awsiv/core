<?php

class Virtualbundle extends Cf_controller {

    function __construct() {
        parent::__construct();
        $this->load->library(array('table', 'cf_table', 'pagination', 'form_validation'));
        $this->load->helper('form');
        $this->load->model('virtual_bundle_model');
        $this->carabiner->js('jquery.tablesorter.min.js');
        $this->carabiner->js('picnet.jquery.tablefilter.js');
        $this->carabiner->js('jquery.tablesorter.pager.js');
        $this->carabiner->js('widgets/policyfinder.js');
        $this->carabiner->js('widgets/notes.js');
    }

    function details($handle=NULL, $user=NULL) {
        $this->carabiner->css('tabs-custom.css');


        if (is_null($handle)) {
            $handle = isset($_POST['handle']) ? $_POST['handle'] : NULL;
        } else {
            $handle = urldecode($handle);
        }

        if (is_null($user)) {
            $user = $this->session->userdata('username');
        }

        $handleData = $this->virtual_bundle_model->getVirtualBundleDetails($handle);
        $handleStatus = $this->virtual_bundle_model->getVirtualBundleStatus($handle);
        $promiseDetails = $this->virtual_bundle_model->getVirtualBundlePromises( $handle,$user);

        $bc = array(
            'title' => 'virtual bundle',
            'url' => 'virtualbundle/details/' . $handle,
            'isRoot' => false,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'handle' => $handle,
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_promise') . " " . $handle,
            'bundle_data' => $handleData,
            'bundle_status' => $handleStatus,
            'bundle_promises' => $promiseDetails ,
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->template->load('template', 'virtualbundle/detail', $data);
    }

    function search() {
        $data['report_type'] = 'virtual bundle';
        $this->load->view('searchpages/virtualbundle_search', $data);
    }

    function searchresult() {
        
    }

    function manage() {
        $this->carabiner->css('tabs-custom.css');
        $bc = array(
            'title' => 'virtual bundles',
            'url' => 'virtualbundle/manage',
            'isRoot' => false,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => $this->lang->line('mission_portal_title'),
            'breadcrumbs' => $this->breadcrumblist->display(),
        );
        $this->template->load('template', 'virtualbundle/managebundles', $data);
    }

    function create() {

        $this->form_validation->set_rules('name', 'Virtual Bundlename', 'xss_clean|trim|required');
        $this->form_validation->set_rules('promises', 'Promises', 'xss_clean|trim|required');
        $this->form_validation->set_rules('description', 'Description', 'xss_clean|trim|required');
        $this->form_validation->set_error_delimiters('<span>', '</span><br/>');
        if ($this->form_validation->run() == FALSE) {
            $this->output->set_status_header('404', "Cannot create virtual bundle");
            echo validation_errors();
        } else {
            $inputs = array(
                'username' => $this->session->userdata('username'),
                'name' => $this->input->post('name'),
                'hostclass' => $this->input->post('hostclass') ? $this->input->post('hostclass') : "",
                'description' => $this->input->post('description') ? $this->input->post('description') : "",
                'promises' => $this->input->post('promises')
            );
            try {
                $result = $this->virtual_bundle_model->createVirtualBundle($inputs);
                if ($result) {
                    echo sprintf('Virtual bundle ( %s ) sucessfully created.', $inputs['name']);
                    ;
                } else {
                    $this->output->set_status_header('500', "Cannot create virtual bundle" . $inputs['name']);
                    echo $result;
                }
            } catch (Exception $e) {
                $this->output->set_status_header('500', "Cannot create virtual bundle. exception occured" . $inputs['name']);
                echo $e->getMessage();
            }
        }
    }

    function myBundles() {
        try {
            $page_number = 1;
            $getparams = $this->uri->uri_to_assoc(3);
            $rows = isset($getparams['rows']) ? $getparams['rows'] : ($this->input->post('rows') ? $this->input->post('rows') : $this->setting_lib->get_no_of_rows());
            if (is_numeric($rows)) {
                $rows = (int) $rows;
            } else {
                $rows = 20;
            }
            $page_number = isset($getparams['page']) ? $getparams['page'] : 1;
            $data = array(
                'report_title' => 'virtual bundle',
                'current' => $page_number,
                'number_of_rows' => $rows,
                'params' => '',
                'url' => 'virtualbundle/myBundles/'
            );
            $data['report_result'] = $this->virtual_bundle_model->getVirtualBundleData(null, $this->session->userdata('username'), $rows, $page_number);
            $this->load->view('searchpages/default_result_view', $data);
        } catch (Exception $e) {
            echo "<p class=\"error\">Exception occured while trying to fetch virtual bundles</p>";
        }
    }

}

?>
