<?

class Visual extends Cf_Controller {

    function Visual() {
        parent::__construct();

        // inject the required js files
        $scripts = array('<!--[if IE]><script language="javascript" type="text/javascript" src=="' . get_scriptdir() . 'flot/excanvas.min.js">  </script><![endif]-->
            ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.min.js"> </script>
                ',
            '<script language="javascript" type="text/javascript" src="' . get_scriptdir() . 'flot/jquery.flot.magnifiedview.js"> </script>
                ',
            '<link href="' . get_cssdir() . 'jquery-ui-1.8.2.custom.css" rel="stylesheet" media="screen" />
                ');


        $this->template->set('injected_item', implode('', $scripts));
    }

    function vital($hostkey=NULL) {
        $hostkey = isset($_POST['hostkey']) ? $_POST['hostkey'] : $hostkey;
        $data = array(
            'title' => "Cfengine Mission Portal - vital signs",
            'title_header' => "vital signs",
            'nav_text' => "Status : vital signs",
            'status' => "current",
            //'ret' => cfpr_performance_analysis($hostkey),
            'hostKey' => $hostkey
        );


        if ($hostkey != 'none') {
            $graphdata = cfpr_performance_analysis($hostkey);
            $convertData = json_decode($graphdata, true);

            if (is_array($convertData)) {
                //var_dump($convertData);
                $data['performanceData'] = $convertData;
                $this->template->load('template', 'visualization/vital', $data);
            }
        } else {
             $this->template->load('template','/visualization/nohost',$data);
        }
    }

    function vitaldetail() {
        $params = $this->uri->uri_to_assoc(6);
        $hostkey = $this->uri->segment(4) . '=' . $this->uri->segment(5);
        $view = $params['view'];
        $obs = $params['obs'];
        $nm = $params['nm'];

        $obs_name = cfpr_get_observable_name($obs);
        $host = cfpr_hostname($hostkey);
        $data = array(
            'hostkey' => $hostkey,
        );

        switch ($view) {
            case 'mag':

                $data['title'] = 'Past 4 hours ' . $obs_name . ' on ' . $host;
                $data['path'] = get_hubdir() . '/' . $hostkey . '/' . $nm . '_mag.png';
                $data['ret'] = cfpr_analysis_mag($hostkey, $obs);
                break;
            case 'week':
                $data['title'] = 'Past week\'s ' . $obs_name . ' on ' . $host;
                $data['path'] = get_hubdir() . '/' . $hostkey . '/' . $nm . '_week.png';
                $data['ret'] = cfpr_analysis_week($hostkey, $obs);
                break;
            case 'hist':
                $data['title'] = 'Frequency spectrum for ' . $obs_name . ' on ' . $host;
                $data['path'] = get_hubdir() . '/' . $hostkey . '/' . $nm . '_hist.png';
                $data['ret'] = cfpr_analysis_hist($hostkey, $obs);
                break;
            default:
        }
        $this->load->view('visualization/vital_details', $data);
    }

}