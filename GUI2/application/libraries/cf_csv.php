<?php

class cf_csv {

    var $header = null;
    var $data = null;
    var $filename = 'temp.csv';

    function initialize($header=array(), $data = array(), $filename=null) {
        $this->header = $header;
        $this->data = $data;
        $this->filename = ($filename !== null) ? $filename : $this->filename;
    }

    function setHeader($header=array()) {
        $this->header = $header;
    }

    function setData($data=array()) {
        $this->data = $data;
    }

    function setFileName($name='sample.csv') {
        $this->filename = $name;
    }

    /**
     * function to build the actual csv content from header and data
     * @param string filename
     * @param strind D-Download, F-write to file
     */
    function Output($filename=null, $flag='D') {


        $this->filename = ($filename !== null) ? $filename : $this->filename;

        if (strtoupper($flag) == 'F') {
            $output = fopen($this->filename, 'w');
        } else {
            // output headers so that the file is downloaded rather than displayed
            header('Content-Type: text/csv; charset=utf-8');
            header('Content-Disposition: attachment; filename=' . $this->filename);

            $output = fopen('php://output', 'w');
        }
        if (is_array($this->header) && !empty($this->header)) {
            fputcsv($output, $this->header);
        }

        if (is_array($this->data) && !empty($this->data)) {
            foreach ($this->data as $row)
                fputcsv($output, $row);
        }

        fclose($output);
    }

}

?>