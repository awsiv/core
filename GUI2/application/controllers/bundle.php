<?php

class Bundle extends Cf_Controller
{

    function Bundle()
    {
        parent::__construct();
        $this->load->model(array('promise_model', 'bundle_model', 'report_model'));
    }

    function details()
    {
        $this->carabiner->css('tabs-custom.css');

        $params = $this->uri->uri_to_assoc(3);
        $bundle = isset($params['bundle']) ? urldecode($params['bundle']) : "";
        $type = isset($params['type']) ? $params['type'] : $this->promise_model->getBundleType($bundle);
        $bc = array(
            'title' => $this->lang->line('breadcrumb_bundle'),
            'url' => 'bundle/details/bundle/' . urlencode($bundle) . '/type/' . urlencode($type),
            'isRoot' => false,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $username = $this->session->userdata('username');
        try
        {
            $tmp_arr = $this->promise_model->getPromiseListByBundle($username, $bundle, 0, 0);
            foreach ((array) $tmp_arr['data'] as $item => $value)
            {
                $bundle_list[] = $value[0];
            }


            $data = array(
                'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_bundle'),
                'status' => "current",
                'bundle' => $bundle,
                'allbundles' => $this->bundle_model->getAllBundles($username),
                'args' => $this->bundle_model->getBundleArguments($username, $type, $bundle),
                'classes' => $this->bundle_model->getBundleClassesUsed($username, $type, $bundle),
                'list' => $bundle_list,
                'others' => $this->bundle_model->getBundleListByUsage($username, $bundle),
                'breadcrumbs' => $this->breadcrumblist->display()
            );
        }
        catch (Exception $e)
        {
            $data['error'] = generate_errormessage($e);
        }
        $this->template->load('template', 'bundle/bundle_detail', $data);
    }

}
