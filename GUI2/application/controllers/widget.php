<?php

class Widget extends Cf_Controller {

    function Widget() {
        parent::__construct();
    }

    function hostfinder() {
        $this->data['hostlist'] = json_decode(cfpr_select_hosts("none", ".*", 100));
        $this->load->view('widgets/hostfinder', $this->data);
    }

    function summaryCompliance() {

        $getparams = $this->uri->uri_to_assoc(3);
        $startDate = isset($getparams['start']) ? $getparams['start'] : -1;
        $env = $getparams['env'];
        $stopDate = isset($getparams['stop']) ? $getparams['stop'] : null;

        $this->load->library('cf_table');
        $startDateTimeStamp = $startDate;
        $stopDateTimeStamp = ($stopDate == null) ? ($startDate + (6 * 3600)) : time();
        $environment = $env;

        $this->data['notkept'] = json_decode(cfpr_summarize_notkept(NULL, NULL, $startDateTimeStamp, $stopDateTimeStamp, $environment, 0, 0), true);
        $this->data['repaired'] = json_decode(cfpr_summarize_repaired(NULL, NULL, $startDateTimeStamp, $stopDateTimeStamp, $environment, 0, 0), true);



        $this->data['startDate'] = getDateStatus($startDateTimeStamp, true);
        $this->data['stopDate'] = getDateStatus($stopDateTimeStamp, true);
        $this->load->view('widgets/summaryCompliance', $this->data);
    }

    function search_by_hostname() {
        $hostname = $this->input->post('value');
        $data = "";
        if ($hostname) {
            $data1 = json_decode(cfpr_show_hosts_name('^' . $hostname, NULL, NULL, NULL), true);
            $data2 = json_decode(cfpr_show_hosts_name('^' . strtolower($hostname), NULL, NULL, NULL), true);
            $data = array_merge($data1, $data2);
        } else {
            //last two arguments in the function call are for row no and column no
            $data = json_decode(cfpr_show_hosts_name(NULL, NULL, NULL, NULL), true);
        }
        echo $this->__format_to_html($data, 'hostname');
    }

    function __format_to_html($result, $display) {
        $html = "";
        if (key_exists('data', $result) && count($result['data']) > 0) {
            $html.="<ul class=\"result\">";
            foreach ($result['data'] as $row) {
                if ($display == 'hostname' && strlen($row[0]) > 0)
                    $html.="<li><a href=" . site_url('welcome/host') . "/" . $row[2] . " title=" . $row[2] . ">$row[0] ($row[1])</a></li>";

                if ($display == 'ipaddress' && strlen($row[1]) > 0)
                    $html.="<li><a href=" . site_url('welcome/host') . "/" . $row[2] . " title=" . $row[2] . ">$row[1] ($row[0])</a></li>";
            }
            $html.="</ul>";
        }
        else {
            $html = "No Host Found";
        }
        return $html;
    }

    function search_by_class() {
        
    }

    function search_by_ipaddress() {
        $ipaddress = $this->input->post('value');
        $data = "";
        if ($ipaddress) {
            $data = json_decode(cfpr_show_hosts_ip($ipaddress, NULL, 10, 1), true);
        } else {
            $data = json_decode(cfpr_show_hosts_ip(NULL, NULL, 10, 1), true);
        }
        echo $this->__format_to_html($data, 'ipaddress');
    }

    function cfclasses() {
        $this->load->library('session');
        $arr = json_decode(cfpr_class_cloud($this->session->userdata('lastclasslist')));
        echo json_encode($arr->classes);
    }

    function ajaxlisthost($currentclass=NULL) {
        $filters = $this->input->post('filter');
        if ($filters) {
            $classlist = implode(",", $filters);
            $arr = json_decode(cfpr_class_cloud($classlist));
            $this->session->set_userdata('lastclasslist', $classlist);
            // echo  host_only_table( $arr->hosts);
            echo $this->__format_to_html2($arr->hosts);
        } else {
            $arr = json_decode(cfpr_class_cloud(NULL));
            $this->session->set_userdata('lastclasslist', NULL);
            echo $this->__format_to_html2($arr->hosts);
        }
    }

    function allclasses() {
        $data=cfpr_list_all_classes(NULL, NULL, NULL, NULL);
        sanitycheckjson($data);
    }

    function filterclass() {
        $filter = $this->input->post('filter');
        $data="";
        switch ($filter) {
            case "time":
                $data= cfpr_list_time_classes(NULL, NULL, NULL, NULL);
                break;
            case "ip":
                $data= cfpr_list_ip_classes(NULL, NULL, NULL, NULL);
                break;
            case "soft":
                $data= cfpr_list_soft_classes(NULL, NULL, NULL, NULL);
                break;
            case "all":
                $data= cfpr_list_all_classes(NULL, NULL, NULL, NULL);
                break;
            case "host":
                $data= cfpr_list_host_classes(NULL, NULL, NULL, NULL);
                break;
        }
        sanitycheckjson($data);
    }

    /* used by host finder */

    function __format_to_html2($result) {
        $html = "";

        if (count($result) > 0) {
            $html.="<ul class=\"result\">";
            foreach ($result as $row) {
                $html.="<li><a href=" . site_url('welcome/host') . "/" . $row[1] . " title=" . $row[1] . ">$row[0]</a></li>";
            }
            $html.="</ul>";
        } else {
            $html = "No Host Found";
        }
        return $html;
    }

    function allpolicies() {
        $data=cfpr_policy_finder_by_handle(NULL, true);
        sanitycheckjson($data);
    }

    function search_by_handle() {
        $handle = $this->input->post('filter');
        $reg = $this->input->post('reg');
        if ($reg == "true") {
            $reg = true;
        }
        if ($reg == "false") {
            $reg = false;
        }
        $data = "";
        if ($handle) {
            $data = cfpr_policy_finder_by_handle($handle, $reg);
        } else {
            $data = cfpr_policy_finder_by_handle(NULL, $reg);
        }
       sanitycheckjson($data);
    }

    function search_by_bundle() {
        $bundle = $this->input->post('filter');
        $reg = $this->input->post('reg');
        if ($reg == "true") {
            $reg = true;
        }
        if ($reg == "false") {
            $reg = false;
        }
        $data = "";
        if ($bundle) {
            $data = cfpr_policy_finder_by_bundle($bundle, $reg);
        } else {
            $data = cfpr_policy_finder_by_bundle(NULL, $reg);
        }
       sanitycheckjson($data);
    }

    function search_by_promiser() {
        $promiser = $this->input->post('filter');
        $reg = $this->input->post('reg');
        if ($reg == "true") {
            $reg = true;
        }
        if ($reg == "false") {
            $reg = false;
        }
        $data = "";
        if ($promiser) {
            $data = cfpr_policy_finder_by_promiser($promiser, $reg);
        } else {
            $data = cfpr_policy_finder_by_promiser(NULL, $reg);
        }
        sanitycheckjson($data);
    }

    function allreports() {
        $data= cfpr_select_reports(".*", 100);
        sanitycheckjson($data);
    }

    function insertworkinglogs() {
        $this->load->library('userdata');
        $message = $this->input->post('message');
        $id = $this->userdata->insert_personal_working_log($message);
        $noteshtml = "";
        $working_notes = $this->userdata->get_personal_working_notes($this->session->userdata('username'));
        foreach ($working_notes as $note) {
            $note = $note['working_on'];
            $noteshtml.= "<li><span class=\"wrknoteslblorg\">Me:</span> $note </li>";
        }
        echo $noteshtml;
    }

    function getworkinglatestlogs() {
        $this->load->library('userdata');
        $users = getonlineusernames();
        $html = "";
        foreach ($users as $user) {
            $html.='<li>' . $user . ' : ' . $this->userdata->get_personal_working_log_latest($user) . '</li>';
        }
        echo $html;
    }

    function tracker() {
        // compliance summary meter
        $envList = cfpr_environments_list();
        //$envListArray = json_decode($envList);
        $data['envList'] = $envList;
        $this->load->view('widgets/tracker', $data);
    }

    function getbreadcrumbs() {
        $this->breadcrumblist->checkurl($this->input->post('url'));
        echo $this->breadcrumblist->display();
    }

}

?>
