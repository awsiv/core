<?php

/**
 * Generates HTML table from JSON data structure for report generation
 */
class cf_table {


    

    public function __construct() {
        //;
        $CI =& get_instance();
        $CI->load->helper('table');
    }

    function geterateTable($jsonData) {

        $this->table->set_heading(array_keys($result['meta']['header']));
        $heading = "";
        if (count($result['data']) > 0) {
            foreach ($result['data'] as $row) {
                $temp = array();
                foreach ($result['meta']['header'] as $key => $value) {
                    if (!is_array($value)) {
                        if (strtolower($key) == "lastseen" || strtolower($key) == "last seen" || strtolower($key) == "time" || strtolower($key) == "last verified")
                            array_push($temp, getDateStatus($row[$value]));
                        else
                            array_push($temp, $row[$value]);
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
                $this->table->add_row($temp);
            }
        }
    }

}

?>
