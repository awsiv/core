<?php

class pdfreports extends Cf_Controller {

    var $reportGenerator = '';
    public $storeDir = './tmp/';
    public $truncateWarning = null;

    function __construct() {
        parent::__construct();
        $this->load->library('cf_pdf');
        $this->load->library('cf_csv');
        $this->reportGenerator = $this->cf_pdf;

        $this->predefinedKeys = array(
            'hostkey',
            'type',
            'search',
            'class_regex',
            'days',
            'months',
            'years',
            'address',
            'ago',
            'version',
            'arch',
            'cal',
            'hours_deltafrom',
            'hours_deltato',
            'to',
            'from',
            'subject',
            'message',
            'diff',
            'scope',
            'lval',
            'rval',
            'state',
            'key',
            'rf',
            'long_term'
        );
    }

    function index() {
        $this->load->library('email');
        $params = $this->uri->uri_to_assoc(3, $this->predefinedKeys);
        $report_type = isset($params['type']) ? urldecode($params['type']) : $this->input->post('type');

        $report_format = 'pdf';
        if (isset($params['rf']) && (trim($params['rf']) != '')) {
            $report_format = trim($params['rf']);
        }

        if ($this->input->post('rf') != null) {
            $report_format = $this->input->post('rf');
        }

        $report_download = isset($params['download']) && trim($params['download'] != '') ? $params['download'] : $this->input->post('download');

        // check if its all download or current page download only 
        if ($report_download == 1) {
            // it all download so pagination paramteres are null 
            $params['page'] = null;
            $params['rows'] = null;
        } else {
            // check the current page and rows
            $params['page'] = $this->input->post('page');
            $params['rows'] = $this->input->post('rows');
        }


        if (trim($report_format) === 'pdf') {
            $filename = 'Nova_' . preg_replace('/ /', '_', $report_type) . "-" . date('m-d-Y-His') . '.pdf';

            $pdf = $this->cf_pdf;
            $pdf->PDFSetReportName($report_type);
            $pdf->PDFSetTableTitle('DATA REPORTED');
            $pdf->AliasNbPages();
            $pdf->SetFont('Arial', '', 14);
            $pdf->AddPage();
            $this->reportGenerator = $pdf;
        } else if (trim($report_format) === 'csv') {
            $filename = 'Nova_' . preg_replace('/ /', '_', $report_type) . "-" . date('m-d-Y-His') . '.csv';
            $this->cf_csv->setFileName($filename);
            $this->reportGenerator = $this->cf_csv;
        }
        // we will use username for RBAC
        $username =  $this->session->userdata('username');
        
        switch ($report_type) {
            case "bundle-profile":
                $this->rpt_bundle_profile($username, $params['hostkey'], $params['search'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "business-value":
                $this->rpt_business_value($username, $params['hostkey'], $params['days'], $params['months'], $params['years'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "contexts":
                $this->rpt_class_profile($username, $params['hostkey'], $params['search'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "promise-compliance":
                $this->rpt_compliance_promises($username, $params['hostkey'], $params['search'], $params['state'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "compliance-summary":
                $this->rpt_compliance_summary($username, $params['hostkey'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "file-change-log":
                $this->rpt_filechange_log($username, $params['hostkey'], $params['search'], $params['class_regex'], $params['long_term'], $params['rows'], $params['page']);
                break;

            case "neighbors":
                $this->rpt_lastsaw_hosts($username, $params['hostkey'], $params['key'], $params['search'], $params['address'], $params['ago'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "patches-available":
                $this->rpt_patches_available($username, $params['hostkey'], $params['search'], $params['version'], $params['arch'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "patches-installed":
                $this->rpt_patch_status($username, $params['hostkey'], $params['search'], $params['version'], $params['arch'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "benchmarks":
                $this->rpt_performance($username, $params['hostkey'], $params['search'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "promises-repaired-summary":
                $this->rpt_promise_repaired_summary($username, $params['hostkey'], $params['search'], $params['hours_deltafrom'], $params['hours_deltato'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "promises-repaired-log":
                $this->rpt_repaired_log($username, $params['hostkey'], $params['search'], $params['hours_deltafrom'], $params['hours_deltato'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "promises-not-kept-summary":
                $this->rpt_promise_notkept_summary($username, $params['hostkey'], $params['search'], $params['hours_deltafrom'], $params['hours_deltato'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "promises-not-kept-log":
                $this->rpt_promise_notkept($username, $params['hostkey'], $params['search'], $params['hours_deltafrom'], $params['hours_deltato'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "setuid-programs":
                $this->rpt_setuid($username, $params['hostkey'], $params['search'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "software-installed":
                $this->rpt_software_installed($username, $params['hostkey'], $params['search'], $params['version'], $params['arch'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "values":
                $this->rpt_variables($username, $params['hostkey'], $params['search'], $params['scope'], $params['lval'], $params['rval'], $params['var_type'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "file-change-diffs":
                $this->rpt_filediffs($username, $params['hostkey'], $params['search'], $params['diff'], $params['cal'], $params['class_regex'], $params['long_term'], $params['rows'], $params['page']);
                break;

            case "virtual-bundles":
                $name = isset($params['name']) ? $params['name'] : null;
                $allUser = isset($params['all_user']) ? $params['all_user'] : null;
                $this->rpt_virtualBundles($name, $allUser, $params['rows'], $params['page']);
                break;
        }

        $pdf_action = key_exists('pdfaction', $params) ? $params['pdfaction'] : $this->input->post('pdfaction');
        # get email parameters from ajax query
        if ($pdf_action == 'email') {
            try {
                $to = $_POST['to'];
                $from = $_POST['from'];
                $subject = $_POST['subject'];
                $msg = $_POST['message'];
                // write the file 
                // check for directory
                $this->checkTempDir();
                $filename = $this->storeDir . $filename;
                $doc = $this->reportGenerator->Output($filename, 'F');
                if (trim($to) === '') {
                    throw new Exception('Email address is empty.');
                }
                $this->emailReport($filename, $to, $from, $subject, $msg);
            } catch (Exception $e) {
                $this->output->set_status_header('400', $e->getMessage());
                echo $e->getMessage();
                exit();
            }
        } else {
            try {
                $this->checkTempDir();
                $filepath = $this->storeDir . $filename;
                $this->reportGenerator->Output($filepath, "F");
                $downloadLink = site_url() . "/pdfreports/download/file/$filename/action/download";
                $msg = sprintf("<a href='%s' target='_blank'>Click here to download the file.</a>", $downloadLink);


                if ($this->truncateWarning !== null)
                    $msg = $msg . '<p class="warning">' . $this->truncateWarning . '</p>';
                $retData = array('message' => $msg);
                $jsonReturn = json_encode($retData);
                echo $jsonReturn;
                exit();
            } catch (Exception $e) {
                $this->output->set_status_header('400', $e->getMessage());
                echo $e->getMessage();
                exit();
            }
        }
    }

    /**
     * Checks if the tmp directory exist and is writable 
     * @throws Exception if not writable
     */
    function checkTempDir() {
        if (!file_exists($this->storeDir)) {
            if (!mkdir($this->storeDir, 0777)) {
                log_message('error', 'Please make sure the tmp directory in web root is present');
                throw new Exception('"tmp" directory in web root not found.');
            }
        }
        if (!is_writable($this->storeDir)) {
            log_message('error', 'Please make sure the tmp directory in web root is writeable');
            throw new Exception('"tmp" directory in web root is not writable ');
        }
    }

    /**
     * Emails the pdf
     * @param <type> $pdf_filename
     * @param <type> $to
     * @param <type> $from
     * @param <type> $subject
     * @param <type> $message
     */
    function emailReport($filename, $to, $from, $subject, $message) {

        // attachment name
        // encode data (puts attachment in proper format)

        $this->email->from($from);
        $this->email->to($to);

        $this->email->subject($subject);
        $this->email->message('Email from cfengine.');
        $this->email->attach($filename);

        if (!$this->email->send()) {
            // error sending mail
            // SOMETHING WENT WRONG WHILE ADDITION
            $this->output->set_status_header('400', 'Something went wrong while sending mail.');
            echo 'Something went wrong while sending mail';
            exit;
        }

        $msg = 'Mail has been sent to the given address.';
        if ($this->truncateWarning)
            $msg .= $msg . '<p class="warning>' . $this->truncateWarning . '</p>';

        $retData = array('message' => $msg);
        $jsonReturn = json_encode($retData);

        echo $jsonReturn;
        @unlink($filename);
        exit();
    }

    /**
     * Function to download the given file and cleanup
     */
    function download() {
        $this->load->helper('download');
        $params = $this->uri->uri_to_assoc(3);
        $filename = isset($params['file']) ? urldecode($params['file']) : null;
        if (is_null($filename)) {
            show_404();
        }
        $downloadFile = $this->storeDir . $filename;
        $data = file_get_contents($downloadFile); // Read the file's contents
        unlink($downloadFile); // remove the file from the server
        force_download($filename, $data);
    }

    /**
     * Checks if there is data truncation warning from the C-API and set the warning  
     * @param array $result data received from C-API 
     */
    function checkForDataTruncation($result) {
        if (is_array($result) && key_exists('truncated', $result['meta'])) {
            $this->truncateWarning = $result['meta']['truncated'];
        }
    }

   /**
    * Changes the timestamp to date format
    * @param array $data 
    * @param type $index array index of the timestamp field
    * @return array
    */
    function changeDateFields($data, $index) {
        foreach ($data as &$row) {
            $row[$index] = getDateStatus($row[$index], false, true);
        }
        return $data;
    }

    /**
     * Removes notes field from the data
     * @param type $data
     * @param type $header
     * @return array 
     */
    function removeNotesField($data, $header) {
        // check for notes if present remove the field from the data
        if (array_key_exists('Note', $header)) {
            $index = $header['Note']['index'];
            // remove the notes data from the array 
            foreach ($data as &$row) {
                unset($row[$index]);
            }
        }
        return $data;
    }

    function rpt_bundle_profile($username, $hostkey, $search, $class_regex, $rows = 0, $page_number = 0) {

        $header = array('Host', 'Bundle', 'Last verified', 'Hours Ago', 'Avg interval', 'Uncertainty');
        $ret = cfpr_report_bundlesseen($username, $hostkey, $search, true, $class_regex, "last-verified", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);
        $this->checkForDataTruncation($jsondata);
        $data1 = $jsondata['data'];
        $header = ($jsondata['meta']['header']);

        $data1 = $this->removeNotesField($data1, $header);
        unset($header['Note']);
        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 2);

        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('bundle profile');

            $pdf->PDFSetDescription($desc);

            $cols = count($header);
            $col_len = array(24, 23, 23, 10, 10, 10); #in percentage    
            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);

            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1, 'bundle_profile_report.csv');
        }
    }

    function rpt_business_value($username, $hostkey, $days, $months, $years, $class_regex, $rows = 0, $page_number = 0) {

        $ret = cfpr_report_value($username, $hostkey, $days, $months, $years, $class_regex, "day", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = ($jsondata['meta']['header']);

        $data1 = $this->removeNotesField($data1, $header);
        unset($header['Note']);
        $header = array_keys($header);

        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('business value report');
            $pdf->PDFSetDescription($desc);
            $cols = count($header);
            $col_len = array(24, 19, 19, 19, 19);
            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);

            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_class_profile($username, $hostkey, $search, $class_regex, $rows = 0, $page_number = 0) {

        $header = array('Host', 'Class Context', 'Occurs with probability', 'Uncertainty', 'Last seen');

        $ret = cfpr_report_classes($username, $hostkey, $search, true, array($class_regex), array(),"last-seen", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $data1 = $this->removeNotesField($data1, $header);
        unset($header['Note']);
        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 4);


        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('class profile');
            $pdf->PDFSetDescription($desc);
            $cols = count($header);
            $col_len = array(28, 28, 18, 10, 16);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_promise_notkept($username, $hostkey, $search, $hours_deltafrom, $hours_deltato, $class_regex = '', $rows = 0, $page_number = 0) {

        $header = array('Host', 'Promise Handle', 'Report', 'Time');

        $ret = cfpr_report_notkept($username, $hostkey, $search, intval($hours_deltafrom), intval($hours_deltato), $class_regex,"time", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $data1 = $this->removeNotesField($data1, $header);
        unset($header['Note']);
        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 3);


        if ($this->reportGenerator instanceof cf_pdf) {
            $col_len = array(15, 21, 49, 15);
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('promises not kept log');
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);

            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, count($header), $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_promise_notkept_summary($username, $hostkey, $search, $hours_deltafrom, $hours_deltato, $class_regex = '', $rows = 0, $page_number = 0) {

        $ret = cfpr_summarize_notkept($username, $hostkey, $search, intval($hours_deltafrom), intval($hours_deltato), $class_regex, "time", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $header = array_keys($header);

        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('promises not kept summary');
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $col_len = array(25, 55, 20);
            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, count($header), $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_promise_repaired_summary($username, $hostkey, $search, $hours_deltafrom, $hours_deltato, $class_regex = '', $rows = 0, $page_number = 0) {
        $header = array('Promise Handle', 'Report', 'Occurrences');

        $ret = cfpr_summarize_repaired($username, $hostkey, $search, intval($hours_deltafrom), intval($hours_deltato), $class_regex, "time", true, $rows, $page_number);

        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $header = array_keys($header);
        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('promises repaired summary');
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $col_len = array(25, 55, 20);
            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, count($header), $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_compliance_promises($username, $hostkey, $search, $state, $class_regex, $rows = 0, $page_number = 0) {
        $header = array('Host', 'Promise Handle', 'Last known state', 'Probability kept', 'Uncertainty', 'Last seen');

        $ret = cfpr_report_compliance_promises($username, $hostkey, $search, $state, true, $class_regex, "last-seen", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 5);

        $cols = count($header);
        $col_len = array(21, 24, 14, 14, 12, 15);


        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('compliance by promise');
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);

            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_compliance_summary($username, $hostkey, $class_regex, $rows = 0, $page_number = 0) {
        $header = array('Host', 'Policy', 'Kept', 'Repaired', 'Not kept', 'Last seen');


        $ret = cfpr_report_compliance_summary($username, $hostkey, NULL, -1, -1, -1, -1, ">", $class_regex, "last-seen", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 5);

        $cols = count($header);
        $col_len = array(25, 33, 8, 8, 8, 18);

        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('compliance summary');
            $pdf->PDFSetDescription($desc);
            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);

            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_filechange_log($username, $hostkey, $search, $class_regex, $longterm_data, $rows = 0, $page_number = 0) {
        $cols = 3;
        $col_len = array(33, 34, 33);
        $header = array('Host', 'File', 'Time of Change');

        if ($longterm_data) {
            $data['report_result'] = cfpr_report_filechanges_longterm($username, $hostkey, $search, true, -1, ">", $class_regex, "time", true, $rows, $page_number);
        } else {
            $data['report_result'] = cfpr_report_filechanges($username, $hostkey, $search, true, -1, ">", $class_regex, "time", true, $rows, $page_number);
        }

        $ret = $data['report_result'];

        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $data1 = $this->removeNotesField($data1, $header);
        unset($header['Note']);

        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 2);


        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('file change log');
            $pdf->PDFSetDescription($desc);
            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_lastsaw_hosts($username, $hostkey, $key, $name, $address, $ago, $class_regex, $rows = 0, $page_number = 0) {
        $header = array('Host', 'Initiated', 'IP Address', 'Remote Host', 'Last Seen', 'Hours Ago', ' Avg Interval', 'Uncertainty', 'Remote Host Key');

        $ret = cfpr_report_lastseen($username, $hostkey, $key, $name, $address, $ago, true, $class_regex, "last-seen", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 4);

        $cols = count($header);
        $col_len = array(14, 8, 10, 14, 12, 7, 7, 7, 21);

        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('last saw hosts');
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $pdf->SetFont('Arial', '', 6);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_patches_available($username, $hostkey, $search, $version, $arch, $class_regex, $rows = 0, $page_number = 0) {

        $ret = cfpr_report_patch_avail($username, $hostkey, $search, $version, $arch, true, array($class_regex), "hostname", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];
        $data1 = $this->changeDateFields($data1, 4);

        $header = array_keys($header);

        $cols = count($header);

        $col_len = array(30, 30, 10, 10, 20);

        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('patches available');
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_patch_status($username, $hostkey, $search, $version, $arch, $class_regex, $rows = 0, $page_number = 0) {

        $ret = cfpr_report_patch_in($username, $hostkey, $search, $version, $arch, true, array($class_regex), array(), "hostname", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 4);

        $cols = count($header);
        $col_len = array(30, 30, 10, 10, 20);


        if ($this->reportGenerator instanceof cf_pdf) {
            $desc = cfpr_report_description('patch status');
            $pdf = $this->reportGenerator;
            $pdf->PDFSetDescription($desc);
            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_software_installed($username, $hostkey, $search, $version, $arch, $class_regex, $rows = 0, $page_number = 0) {

        $ret = cfpr_report_software_in($username, $hostkey, $search, $version, $arch, true, array($class_regex), array(), "hostname", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 4);



        $cols = count($header);
        $col_len = array(30, 30, 10, 10, 20);

        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('software installed');
            $pdf->PDFSetDescription($desc);
            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_performance($username, $hostkey, $search, $class_regex, $rows = 0, $page_number = 0) {

        $ret = cfpr_report_performance($username, $hostkey, $search, true, $class_regex, "last-performed", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $data1 = $this->removeNotesField($data1, $header);
        unset($header['Note']);
        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 5);


        $cols = count($header);
        $col_len = array(19, 38, 7.5, 7.5, 10, 18);


        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('performance');
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_repaired_log($username, $hostkey, $search, $hours_deltafrom, $hours_deltato, $class_regex, $rows = 0, $page_number = 0) {
        $header = array('Host', 'Promise Handle', 'Report', 'Time');

        $ret = cfpr_report_repaired($username, $hostkey, $search, intval($hours_deltafrom), intval($hours_deltato), $class_regex, "time", true, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];


        $data1 = $this->removeNotesField($data1, $header);
        unset($header['Note']);
        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 3);


        $cols = count($header);
        $col_len = array(19, 19, 43, 19);


        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('promises repaired log');
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_virtualBundles($name, $allUser, $page = 0, $rows = 0) {
        $this->load->model('virtual_bundle_model');
        $cols = 4;
        $col_len = array(19, 19, 43, 19);
        $header = array('Virtual bundle', 'Promises', 'Hosts', 'Compliance');

        $ret = $this->virtual_bundle_model->getVirtualBundleData($name, $allUser);
        $dataArray = json_decode($ret, true);
        $data1 = $dataArray['data'];

        if ($this->reportGenerator instanceof cf_pdf) {

            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('Virtual bundles');
            $pdf->PDFSetDescription($desc);
            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);
            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

    function rpt_variables($username, $hostkey, $search, $scope, $lval, $rval, $type, $class_regex, $rows = 0, $page_number = 0) {
        $header = array();

        if ($hostkey == NULL) {
            $ret = cfpr_report_vars($username, NULL, $scope, $lval, $rval, $type, true, array($class_regex), array(), "var-name", true, $rows, $page_number);
        } else {
            $ret = cfpr_report_vars($username, $hostkey, NULL, $search, NULL, NULL, true, array($class_regex), array(), "var-name", true, $rows, $page_number);
        }

        $dataArray = json_decode($ret, true);

        $data1 = $dataArray;

        foreach ($data1 as $topic => &$subtopic) {
            if ($topic == 'meta')
                continue;
            $head = array_keys($subtopic['header']);
            $newArray[][] = $topic;
            $newArray[] = $head;
            // change the time stamp
            $subtopic['data'] = $this->changeDateFields($subtopic['data'], 4);

            $newArray = array_merge($newArray, $subtopic['data']);
        }

        $cols = 5;
        $col_len = array(20, 20, 20, 20, 20);


        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('variables');
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);

            $pdf->SetFont('Arial', '', 9);
            $pdf->drawMultiTable($data1, $col_len);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $newArray);
        }
    }

    function rpt_filediffs($username, $hostkey, $search, $diff, $cal, $class_regex, $longterm, $page = 0, $rows = 0) {

        if ($longterm) {
            $data['report_result'] = cfpr_report_filediffs_longterm($username, NULL, $search, $diff, true, $cal, ">", $class_regex, "time", true, $page, $rows);
        } else {
            $data['report_result'] = cfpr_report_filediffs($username, NULL, $search, $diff, true, $cal, ">", $class_regex, "time", true, $page, $rows);
        }
        $result = json_decode($data['report_result'], true);
        $newFormat = array();
        foreach ($result['data'] as $parentIndex => $data) {

            foreach ($data[3] as $index => $subArray) {
                $newIndex = $parentIndex . '-' . $index;
                $newFormat[$newIndex][0] = $data[0];
                $newFormat[$newIndex][1] = $data[1];
                $newFormat[$newIndex][2] = date('c', $data[2]);
                $newFormat[$newIndex][3] = $subArray[0];
                $newFormat[$newIndex][4] = $subArray[1];
                $newFormat[$newIndex][5] = $subArray[2];
            }
        }
        $header = array('Host', 'File', 'Change detected at', 'Change', 'Line number', 'File');

        $cols = 6;
        $col_len = array(20, 15, 15, 10, 10, 30);

        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('file change diffs');
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);

            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($newFormat, count($header), $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $newFormat);
        }
    }

    function rpt_setuid($username, $hostkey, $search, $class_regex, $rows = 0, $page_number = 0) {
        $header = array('Host', 'Type', 'Name', 'Value');
        $ret = cfpr_report_setuid($username, $hostkey, $search, true, $class_regex, "hostname", true, $rows, $page_number);

        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];
        $header = array_keys($header);
        $cols = count($header);
        $col_len = array(30, 70);

        if ($this->reportGenerator instanceof cf_pdf) {
            $pdf = $this->reportGenerator;
            $desc = cfpr_report_description('setuid gid root programs');
            $pdf->PDFSetDescription($desc);
            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);

            $pdf->SetFont('Arial', '', 9);
            $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
        } else if ($this->reportGenerator instanceof cf_csv) {
            $this->cf_csv->initialize($header, $data1);
        }
    }

}