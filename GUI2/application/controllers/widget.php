<?php

class Widget extends CI_Controller {

    function Widget() {
        parent::__construct();
    }

    function hostfinder() {
        $this->data['hostlist'] = json_decode(cfpr_select_hosts("none", ".*", 100));
        $this->load->view('widgets/hostfinder', $this->data);
    }

    function summaryCompliance($startDate) {

        $this->load->library('cf_table');
        $startDateTimeStamp = $startDate;
        $stopDateTimeStamp = $startDate + (6 * 3600);
        $this->data['notkept'] = json_decode(cfpr_summarize_notkept(NULL, NULL, $startDateTimeStamp, $stopDateTimeStamp, NULL), true);
        $this->data['repaired'] = json_decode(cfpr_summarize_repaired(NULL, NULL, $startDateTimeStamp, $stopDateTimeStamp, NULL), true);
        $this->data['startDate'] = getDateStatus($startDateTimeStamp, true);
        $this->data['stopDate'] = getDateStatus($stopDateTimeStamp, true);

        $this->load->view('widgets/summaryCompliance', $this->data);
    }


        function search_by_hostname()
        {
            $hostname=$this->input->post('value');
            $data="";
            if($hostname)
            {
               $data=json_decode(cfpr_show_hosts_name($hostname,NULL,10,1),true);
               $response=$this->__format_to_html($data);
            }
            else
            {
               $data=json_decode(cfpr_show_hosts_name(NULL,NULL,10,1),true);
            }
           echo $this->__format_to_html($data);
        }

        function __format_to_html($result)
        {
          $html="";

          if (key_exists('data', $result) && count($result['data']) > 0) {
              $html.="<ul>";
                foreach ($result['data'] as $row) {
                    $html.="<li><a href=".site_url('welcome/host')."/".$row[2].">$row[1]</a></li>";
                }
               $html.="</ul>";
            }
          else
          {
              $html="No Host Found";
          }
          return $html;
        }

        function search_by_class()
        {


        }

        function search_by_ipaddress()
        {
            $ipaddress=$this->input->post('value');
            $data="";
            if($ipaddress)
            {
               $data=json_decode(cfpr_show_hosts_ip($ipaddress,NULL,10,1),true);
               $response=$this->__format_to_html($data);
            }
            else
            {
               $data=json_decode(cfpr_show_hosts_ip(NULL,NULL,10,1),true);
            }
           echo $this->__format_to_html($data);
        }

      function cfclasses()
      {
         $this->load->library('session');
         $arr=json_decode(cfpr_class_cloud($this->session->userdata('lastclasslist')));
         echo json_encode($arr->classes);
      }

      function ajaxlisthost($currentclass=NULL)
        {
         $filters=$this->input->post('filter');
             if($filters)
             {
                $classlist=implode(",",$filters);
                $arr=json_decode(cfpr_class_cloud($classlist));
                $this->session->set_userdata('lastclasslist',$classlist);
               // echo  host_only_table( $arr->hosts);
              echo  $this->__format_to_html2($arr->hosts);
             }
             else
             {
               $arr=json_decode(cfpr_class_cloud(NULL));
               $this->session->set_userdata('lastclasslist',NULL);
              echo  $this->__format_to_html2($arr->hosts);
             }
       }

       function __format_to_html2($result)
        {
          $html="";

          if (count($result) > 0) {
              $html.="<ul>";
                foreach ($result as $row) {
                    $html.="<li><a href=".site_url('welcome/host')."/".$row[1].">$row[0]</a></li>";
                }
               $html.="</ul>";
            }
          else
          {
              $html="No Host Found";
          }
          return $html;
        }
}

?>
