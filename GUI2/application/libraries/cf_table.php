<?php

/**
 * Generates HTML table from JSON data structure for report generation
 */
class cf_table {

    var $dateTimeFields = array();

    public function __construct() {
        $this->CI = &get_instance();
        $this->CI->load->library('table');

        // these array are the report titles where date should not be coloured 
        $this->ignoreDateColor = array('Promises not kept log','Promises repaired log');

        // these are the table fields that should be formatted as date and time
        $this->dateTimeField = array('lastseen', 'last seen', 'time', 'last verified', 'change detected at');
    }

    function generateReportTable($result,$report_title = '') {
        $return = '';
        if (is_array($result)) {
            $this->CI->table->set_heading(array_keys($result['meta']['header']));
            $heading = "";
            if (count($result['data']) > 0) {
                foreach ($result['data'] as $row) {
                    $temp = array();
                    foreach ($result['meta']['header'] as $key => $value) {
                        if (!is_array($value)) {
                            if (in_array(strtolower($key), $this->dateTimeField)) {
                                $color = in_array(trim($report_title),$this->ignoreDateColor);
                                array_push($temp, getDateStatus($row[$value],$color));
                            }
                            else{
                                array_push($temp, $row[$value]);
                            }

                        }
                        else {
                            if (strtolower($key) == "note") {
                                $link = site_url("notes") . '/index/';
                                $data_index = $value['index'];
                                // var_dump($row[$data_index] );
                                foreach ($value['subkeys'] as $subkey => $subval) {
                                    $data = trim($row[$data_index][$subval]);
                                    if ($subkey == 'rid') {
                                        $data = urlencode(base64_encode(utf8_encode($data)));
                                    }
                                    if ($data != '') {
                                        $link.="$subkey/$data/";
                                    }
                                }
                                array_push($temp, anchor($link, 'note', 'class=note'));
                            }
                        }
                    }
                    $this->CI->table->add_row($temp);
                }
            }
            $return = $this->CI->table->generate();
        }
        return  $return;
    }

}

?>
