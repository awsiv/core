<?php

class Widget extends Cf_Controller
{

    function Widget()
    {
        parent::__construct();
        $this->load->model(array('host_model', 'class_model', 'report_model', 'bundle_model', 'promise_model', 'environment_model'));
        if (!$this->ion_auth->logged_in())
        {
            $this->output->set_status_header('401', 'Not Authenticated');
            echo $this->lang->line('session_expired');
            exit;
        }
    }

    function hostfinder($page = 1)
    {

        try
        {
            $result = $this->host_model->getHostByName($this->session->userdata('username'), '.*', 100, $page);
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', $e->getMessage());
            echo $e->message();
        }

        if ($page > 1)
        {
            echo $this->__format_to_html($result, 'hostname');
            return;
        }
        if (is_array($result))
        {
            // $this->data['hostlist']= array_msort($result,array('id'=>SORT_ASC),true);
            $this->data['hostlist'] = array_msort($result['data'], array('0' => SORT_ASC), true);
        }
        else
        {
            $this->data['error'] = "Host list cannot be  generated due to invalid json data";
        }
        $this->load->view('widgets/hostfinder', $this->data);
    }

    function summaryCompliance()
    {
        // we will use username for RBAC
        $username = $this->session->userdata('username');

        $getparams = $this->uri->uri_to_assoc(3);
        $startDate = isset($getparams['start']) ? $getparams['start'] : 0;
        $env = $this->input->post('env', TRUE);
        $stopDate = isset($getparams['stop']) ? $getparams['stop'] : null;

        $this->load->library('cf_table');
        $startDateTimeStamp = $startDate;
        $stopDateTimeStamp = ($stopDate == null) ? ($startDate + (6 * 3600)) : $stopDate;

        $includes = isset($getparams['includes']) ? explode(',', urldecode($getparams['includes'])) : array();
        $excludes = isset($getparams['excludes']) ? explode(',', urldecode($getparams['includes'])) : array();

        if ($env)
        {
            $includes = array($env);
        }
        else
        {
            if ($this->input->post('includes') !== FALSE)
            {
                $includes = array_merge($includes, $this->input->post('includes', TRUE));
            }

            if ($this->input->post('excludes') !== FALSE)
            {
                $excludes = $this->input->post('excludes', TRUE);
            }
        }

        $this->data['notkept'] = $this->report_model->getPromisesNotKeptSummary($username, NULL, NULL, '.*', $startDateTimeStamp, $stopDateTimeStamp, $includes, $excludes);
        $this->data['repaired'] = $this->report_model->getPromisesRepairedSummary($username, NULL, NULL, '.*', $startDateTimeStamp, $stopDateTimeStamp, $includes, $excludes);

        $this->data['startDate'] = getDateStatus($startDateTimeStamp, true);
        $this->data['stopDate'] = getDateStatus($stopDateTimeStamp, true);
        $this->load->view('widgets/summaryCompliance', $this->data);
    }

    function search_by_hostname($hostname = null, $page = 1)
    {
        $hostname = $this->input->post('value') ? $this->input->post('value') : urldecode($hostname);
        try
        {
            $data = $this->host_model->getHostByName($this->session->userdata('username'), $hostname, 100, $page);
            echo $this->__format_to_html($data, 'hostname');
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', $e->getMessage());
            echo $e->getMessage();
        }
    }

    function sort_alphabetically_hostname($hostname = null, $page = 1)
    {
        $hostname = $this->input->post('value') ? $this->input->post('value') : urldecode($hostname);
        $data = "";
        $searchhost = '^[' . $hostname . '|' . strtolower($hostname) . ']';
        try
        {
            $data = $this->host_model->getHostByName($this->session->userdata('username'), $searchhost, 100, $page);
            echo $this->__format_to_html($data, 'hostname');
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', $e->getMessage());
            echo $e->getMessage();
        }
    }

    function __format_to_html($result, $display)
    {
        $html = "";
        if (is_array($result) && key_exists('data', $result) && count($result['data']) > 0)
        {
            $result = array_msort($result['data'], array('0' => SORT_ASC), true);
            // $html.="<ul class=\"result\">";
            foreach ($result as $row)
            {
                if ($display == 'hostname' && strlen($row[0]) > 0)
                    $html.="<li><a href=" . site_url('welcome/host') . "/" . $row[0] . " title=" . $row[1] . ">$row[1]</a></li>";

                if ($display == 'ipaddress' && strlen($row[1]) > 0)
                    $html.="<li><a href=" . site_url('welcome/host') . "/" . $row[0] . " title=" . $row[1] . ">$row[2] ($row[1])</a></li>";
            }
            //$html.="</ul>";
        }
        else
        {
            $html = "";
        }
        return $html;
    }

    function search_by_class()
    {

    }

    function search_by_ipaddress($ipregx = null, $page = 1)
    {
        $ipaddress = $this->input->post('value') ? $this->input->post('value') : urldecode($ipregx);
        try
        {
            $data = $this->host_model->getHostByIP($this->session->userdata('username'), $ipaddress, 100, $page);
            echo $this->__format_to_html($data, 'ipaddress');
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', $e->getMessage());
            echo $e->getMessage();
        }
    }

    function cfclasses()
    {
        $arr = json_decode(cfpr_class_cloud($this->session->userdata('lastclasslist')));
        echo json_encode($arr->classes);
    }

    function ajaxlisthost($currentclass = NULL)
    {
        $filters = $this->input->post('filter');
        if ($filters)
        {
            $classlist = implode(",", $filters);
            $arr = json_decode(cfpr_class_cloud($classlist));
            $this->session->set_userdata('lastclasslist', $classlist);
            echo $this->__format_to_html2($arr->hosts);
        }
        else
        {
            $arr = json_decode(cfpr_class_cloud(NULL));
            $this->session->set_userdata('lastclasslist', NULL);
            echo $this->__format_to_html2($arr->hosts);
        }
    }

    function allclasses($page = 1, $alphaSearch = null)
    {
        $username = $this->session->userdata('username');

        $searchletter = null;

        if ($alphaSearch != null)
        {
            $searchletter = "^" . urldecode($alphaSearch) . '.*';
        }
        else
        {
            $searchletter = '.*';
        }


        $includes = $excludes = array();

        if ($this->input->post('includes') !== FALSE)
        {
            $includes = array_merge($includes, $this->input->post('includes', TRUE));
        }

        if ($this->input->post('excludes') !== FALSE)
        {
            $excludes = $this->input->post('excludes', TRUE);
        }

        try
        {
            $classes = $this->class_model->getAllClasses($username, $searchletter, $includes, $excludes, 100, $page);
            echo $classes;
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', $e->getMessage());
            echo($e->getMessage());
        }
    }

    function filterclass($page = 1, $search = null)
    {
        $username = $this->session->userdata('username');
        $searchletter = null;

        if ($search != null)
        {
            $searched = urldecode($search);
            if (preg_match('/^\[\w\|\w\]$/', $searched))
            {
                $searchletter = "^" . $searched . '.*';
            }
            elseif (preg_match('/.*\W+.*/', $search))
            {
                $searchletter = $searched;
            }
            else
            {
                $searchletter = '.*' . $searched . '.*';
            }
        }
        else
        {
            $searchletter = '.*';
        }

        $filter = '';

        //add include/exclude
        $includes = $excludes = array();

        if ($this->input->post('filter') !== FALSE)
        {
            $filter = $this->input->post('filter', TRUE);
        }

        if ($this->input->post('includes') !== FALSE)
        {
            $includes = array_merge($includes, $this->input->post('includes', TRUE));
        }

        if ($this->input->post('excludes') !== FALSE)
        {
            $excludes = $this->input->post('excludes', TRUE);
        }

        $data = "";

        try
        {
            switch ($filter)
            {
                case "time":
                    $data = $this->class_model->getAllTimeClasses($username, $searchletter, $includes, $excludes, 100, $page);
                    break;
                case "ip":
                    $data = cfpr_list_ip_classes(NULL, NULL, NULL, NULL);
                    break;
                case "soft":
                    $data = $this->class_model->getAllSoftClasses($username, $searchletter, $includes, $excludes, 100, $page);
                    break;
                case "all":
                    $data = $this->class_model->getAllClasses($username, $searchletter, $includes, $excludes, 100, $page);
                    break;
                case "host":
                    $data = cfpr_list_host_classes(NULL, NULL, NULL, NULL);
                    break;
            }

            echo $data;
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', $e->getMessage());
            echo($e->getMessage());
        }
    }

    /* used by host finder */

    function __format_to_html2($result)
    {
        $html = "";

        if (count($result) > 0)
        {
            $html.="<ul class=\"result\">";
            foreach ($result as $row)
            {
                $html.="<li><a href=" . site_url('welcome/host') . "/" . $row[1] . " title=" . $row[1] . ">$row[0]</a></li>";
            }
            $html.="</ul>";
        }
        else
        {
            $html = "No Host Found";
        }
        return $html;
    }

    function allpolicies($page = 1)
    {
        try
        {

            $returnedData = $this->promise_model->getPromiseListByHandleRx($this->session->userdata('username'), NULL, 50, $page);
            $showButton = $this->input->post('showButton');
            $showOnlyHandle = trim($this->input->post('showOnlyHandle')) === 'false' ? false : true;
            $showOnlyBundle = trim($this->input->post('showOnlyBundle')) === 'false' ? false : true;
            $viewdata = array(
                'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_report'),
                'showButton' => $showButton,
                'showOnlyHandle' => $showOnlyHandle,
                'showOnlyBundle' => $showOnlyBundle
            );


            $viewdata['viewdata'] = $returnedData;

            $this->load->view('widgets/allpolicies', $viewdata);
        }
        catch (Exception $e)
        {
            show_error_custom($e->getMessage());
        }
    }

    function search_by_handle($page = 1)
    {
        $handle = $this->input->post('filter');
        $showButton = $this->input->post('showButton');
        $showOnlyHandle = trim($this->input->post('showOnlyHandle')) === 'false' ? false : true;
        $showOnlyBundle = trim($this->input->post('showOnlyBundle')) === 'false' ? false : true;
        $viewdata = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_report'),
            'showButton' => $showButton,
            'showOnlyHandle' => $showOnlyHandle,
            'showOnlyBundle' => $showOnlyBundle
        );

        $handle = ($handle) ? $handle . '.*' : '.*';

        try
        {
            $returnedData = $this->promise_model->getPromiseListByHandleRx($this->session->userdata('username'), $handle, 50, $page);
            $viewdata['viewdata'] = $returnedData;
            $this->load->view('widgets/allpolicies', $viewdata);
        }
        catch (Exception $e)
        {
            show_error_custom($e->getMessage());
        }
    }

    function search_by_bundle($page = 1)
    {
        $bundle = $this->input->post('filter');
        $showButton = $this->input->post('showButton');
        $showOnlyHandle = trim($this->input->post('showOnlyHandle')) === 'false' ? false : true;
        $showOnlyBundle = trim($this->input->post('showOnlyBundle')) === 'false' ? false : true;
        $viewdata = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_report'),
            'showButton' => $showButton,
            'showOnlyHandle' => $showOnlyHandle,
            'showOnlyBundle' => $showOnlyBundle
        );

        $data = "";
        $bundle = ($bundle) ? $bundle . '.*' : NULL;
        try
        {
            $data = $this->promise_model->getPromiseListByBundleRx($this->session->userdata('username'), '.*', $bundle, 50, $page);
            $viewdata['viewdata'] = $data;
            $this->load->view('widgets/allpolicies', $viewdata);
        }
        catch (Exception $e)
        {
            show_error_custom($e->getMessage());
        }
    }

    function search_by_bundle_type($page = 1)
    {
        $val = $this->input->post('filter');
        $type = $this->input->post('type');
        $showButton = $this->input->post('showButton');
        $showOnlyHandle = trim($this->input->post('showOnlyHandle')) === 'false' ? false : true;
        $showOnlyBundle = trim($this->input->post('showOnlyBundle')) === 'false' ? false : true;
        $viewdata = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_report'),
            'showButton' => $showButton,
            'showOnlyHandle' => $showOnlyHandle,
            'showOnlyBundle' => $showOnlyBundle
        );


        try
        {
            $val = ($val) ? $val . '.*' : ".*";
            $promises = $this->promise_model->getPromiseListByBundleRx($this->session->userdata('username'), $val, null, 100, $page);
            $viewdata['viewdata'] = $promises;
            $this->load->view('widgets/allpolicies', $viewdata);
        }
        catch (Exception $e)
        {
            show_error_custom($e->getMessage());
        }
    }

    function search_by_promiser($page = 1)
    {
        $promiser = $this->input->post('filter');
        $showButton = $this->input->post('showButton');
        $showOnlyHandle = trim($this->input->post('showOnlyHandle')) === 'false' ? false : true;
        $showOnlyBundle = trim($this->input->post('showOnlyBundle')) === 'false' ? false : true;
        $viewdata = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_report'),
            'showButton' => $showButton,
            'showOnlyHandle' => $showOnlyHandle,
            'showOnlyBundle' => $showOnlyBundle
        );
        try
        {
            $promiser = ($promiser) ? $promiser . '.*' : null;
            $returnedData = $this->promise_model->getPromiseListByPromiserRx($this->session->userdata('username'), $promiser, 100, $page);
            $viewdata['viewdata'] = $returnedData;
            $this->load->view('widgets/allpolicies', $viewdata);
        }
        catch (Exception $e)
        {
            show_error_custom($e->getMessage());
        }
    }

    function allreports()
    {
        try
        {
            $reportsData = $this->report_model->getAllReports();
            $reports = $reportsData['data'];
            $treeview_reports = array();
            foreach ($reports as $report)
            {
                if (key_exists($report['category'], $treeview_reports))
                {
                    array_push($treeview_reports[$report['category']], $report);
                }
                else
                {
                    $treeview_reports[$report['category']] = array();
                    array_push($treeview_reports[$report['category']], $report);
                }
            }
            $json = json_encode($treeview_reports);
            echo $json;
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', $reportsData['error']['message']);
            exit;
        }
    }

    function insertworkinglogs()
    {
        $this->load->library('userdata');
        $message = $this->input->post('message');
        $id = $this->userdata->insert_personal_working_log($message);
        $noteshtml = "";
        $working_notes = $this->userdata->get_personal_working_notes($this->session->userdata('username'));
        foreach ($working_notes as $note)
        {
            $note = $note['working_on'];
            $noteshtml.= "<li><span class=\"wrknoteslblorg\">Me:</span> $note </li>";
        }
        echo $noteshtml;
    }

    function getworkinglatestlogs()
    {
        $this->load->library('userdata');
        $users = getonlineusernames();
        $html = "";
        foreach ($users as $user)
        {
            $html.='<li>' . $user . ' : ' . $this->userdata->get_personal_working_log_latest($user) . '</li>';
        }
        echo $html;
    }

    function tracker()
    {
        // compliance summary meter
        try
        {
            $envList = $this->environment_model->getEnvironmentList($this->session->userdata('username'));
        }
        catch (CFModExceptionRBAC $e)
        {
            show_error($this->lang->line('rbac_exception'), 401);
        }
        catch (Exception $e)
        {
            show_error($this->lang->line('cfmod_exception'), 500);
        }
        //$envListArray = json_decode($envList);
        $data['envList'] = $envList;
        $this->load->view('widgets/tracker', $data);
    }

    function getbreadcrumbs()
    {
        $this->breadcrumblist->checkurl($this->input->post('url'));
        echo $this->breadcrumblist->display();
    }

    function astrolabeAddNodeDialog($label = NULL, $class = NULL)
    {
        $data['label'] = $label;
        $data['class'] = $class;

        $this->load->view('widgets/astrolabeAddNodeDialog', $data);
    }

    function contextfinder()
    {
        $data = array();

        if($this->input->post('html_id') !== FALSE) {
            $data['html_id'] = $this->input->post('html_id', TRUE);
        }

        if($this->input->post('doNotShowButtons') !== FALSE) {
            if (strtolower($this->input->post('doNotShowButtons', TRUE)) == 'false' || $this->input->post('doNotShowButtons', TRUE) == '')
                $data['doNotShowButtons'] = false;
            else
                $data['doNotShowButtons'] = true;
        }

        if($this->input->post('embedded') !== FALSE) {
            if (strtolower($this->input->post('emdedded', TRUE)) == 'false' || $this->input->post('emdedded', TRUE) == '')
                $data['embedded'] = false;
            else
                $data['embedded'] = true;
        }

        if($this->input->post('include_field_name') !== FALSE) {
            $data['include_field_name'] = $this->input->post('include_field_name', TRUE);
        }

        if ($this->input->post('includes') !== FALSE) {
           $data['fields']['left']['values'] = $this->input->post('includes');
        }

        if($this->input->post('exclude_field_name') !== FALSE) {
            $data['include_field_name'] = $this->input->post('include_field_name', TRUE);
        }

        if ($this->input->post('excludes') !== FALSE) {
           $data['fields']['right']['values'] = $this->input->post('excludes');
        }

        $this->load->view('widgets/contextfinder', $data);
    }

    /**
     * Return all bundles except bundles assigned to the role
     *
     * @param rolename string
     *
     * @return json string
     *
     */
    function bundlesNotAssignedToRole($rolename = '')
    {
        $filter = $this->input->post('filter');

        $username = $this->session->userdata('username');
        $all_bundles = $brxi = $brxx = array();

        try
        {
            $all_bundles_tmp = $this->bundle_model->getAllBundles($username);

            if (!empty($all_bundles_tmp))
            {
                // get bundles in normal view - key value array + only single values (like array_unique)
                // in the end we will get array like $arr[$key] = $key
                foreach ($all_bundles_tmp as $item)
                {
                    $all_bundles[$item[1]] = $item[1]; // $item[1] - bundle name
                }
            }

            // if rolename is empty - return all avilable bundles
            if ($rolename == '')
            {
                $bundle = $all_bundles;
            }

            else {
                $role = $this->ion_auth->get_role($this->session->userdata('username'), $rolename);

                $brxi = array_key_exists('bundlerxinlcude', $role) ? $role['bundlerxinlcude'] : "";
                $brxx = array_key_exists('bundlerxexclude', $role) ? $role['bundlerxexclude'] : "";

                //clean spaces and convert to to array
                $brxi = str_replace(' ', '', $brxi);
                $brxi = explode(',', $brxi);

                $brxx = str_replace(' ', '', $brxx);
                $brxx = explode(',', $brxx);


                // do the same for assigned bundles
                $used_bundles_tmp = array_merge((array) $brxi, (array) $brxx);
                $used_bundles = array_combine($used_bundles_tmp, $used_bundles_tmp); // create array as $arr[$key] = $key

                unset($used_bundles_tmp);

                if (!empty($used_bundles))
                {
                    $bundle = arrayRecursiveDiff($all_bundles, $used_bundles);
                }
            }

               // echo json_encode(array_keys($bundle));
            $showButton = $this->input->post('showButton');
            $showOnlyHandle = trim($this->input->post('showOnlyHandle')) === 'false' ? false : true;
            $showOnlyBundle = trim($this->input->post('showOnlyBundle')) === 'false' ? false : true;
            $viewdata = array(

                'showButton' => $showButton,
                'showOnlyHandle' => $showOnlyHandle,
                'showOnlyBundle' => $showOnlyBundle
            );

            // search by regex (letter)
            if ($filter != '')
            {
                $bundle = preg_grep ('/'.$filter.'/', $bundle);
            }


            $viewdata['viewdata'] = array_keys($bundle);

            $this->load->view('widgets/allpolicies', $viewdata);

        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', $e->getMessage());
            echo($e->getMessage());
        }
    }

}

?>
