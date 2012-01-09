<?php

class pdfreports extends Cf_Controller {

    var $reportGenerator = '';
    public $storeDir = './tmp/';

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

    function populateParamsWithDefault($params) {
        $predefinedKeys = array(
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
            'key'
        );

        foreach ($predefinedKeys as $key) {
            if (!isset($params[$key])) {
                $params[$key] = null; #set key with null value
            }
        }
        return $params;
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

        switch ($report_type) {
            case "Bundle profile":
                $this->rpt_bundle_profile($params['hostkey'], $params['search'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Business value report":
                $this->rpt_business_value($params['hostkey'], $params['days'], $params['months'], $params['years'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Class profile":
                $this->rpt_class_profile($params['hostkey'], $params['search'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Compliance by promise":
                $this->rpt_compliance_promises($params['hostkey'], $params['search'], $params['state'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Compliance summary":
                $this->rpt_compliance_summary($params['hostkey'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "File change log":
                $this->rpt_filechange_log($params['hostkey'], $params['search'], $params['class_regex'], $params['long_term'], $params['rows'], $params['page']);
                break;

            case "Last saw hosts":
                $this->rpt_lastsaw_hosts($params['hostkey'], $params['key'], $params['search'], $params['address'], $params['ago'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Patches available":
                $this->rpt_patches_available($params['hostkey'], $params['search'], $params['version'], $params['arch'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Patch status":
                $this->rpt_patch_status($params['hostkey'], $params['search'], $params['version'], $params['arch'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Performance":
                $this->rpt_performance($params['hostkey'], $params['search'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Promises repaired summary":
                $this->rpt_promise_repaired_summary($params['hostkey'], $params['search'], $params['hours_deltafrom'], $params['hours_deltato'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Promises repaired log":
                $this->rpt_repaired_log($params['hostkey'], $params['search'], $params['hours_deltafrom'], $params['hours_deltato'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Promises not kept summary":
                $this->rpt_promise_notkept_summary($params['hostkey'], $params['search'], $params['hours_deltafrom'], $params['hours_deltato'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Promises not kept log":
                $this->rpt_promise_notkept($params['hostkey'], $params['search'], $params['hours_deltafrom'], $params['hours_deltato'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Setuid-gid root programs":
                $this->rpt_setuid($params['hostkey'], $params['search'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Software installed":
                $this->rpt_software_installed($params['hostkey'], $params['search'], $params['version'], $params['arch'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "Variables":
                $this->rpt_variables($params['hostkey'], $params['search'], $params['scope'], $params['lval'], $params['rval'], $params['var_type'], $params['class_regex'], $params['rows'], $params['page']);
                break;

            case "File change diffs":
                $this->rpt_filediffs($params['hostkey'], $params['search'], $params['diff'], $params['cal'], $params['class_regex'], $params['long_term'], $params['rows'], $params['page']);
                break;

            case "Virtual bundles":
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
                if (!file_exists("./tmp")) {
                    mkdir("./tmp", 0777);
                }
                
                if (!is_writable($this->storeDir)) {
                    log_message('error', 'Please make sure the tmp directory in web root is writeable');
                    throw new Exception('directory "tmp" is not writable ');
                }

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
                if (!file_exists("./tmp")) {
                    mkdir("./tmp", 0777);
                }
                if (!is_writable($this->storeDir)) {
                    log_message('error', 'Please make sure the tmp directory in web root is writeable');
                    throw new Exception('directory "tmp" is not writable ');
                }
                $filepath = $this->storeDir . $filename;
                $this->reportGenerator->Output($filepath, "F");
                $downloadLink = site_url() . "/pdfreports/download/file/$filename/action/download";
                $msg = sprintf("<a href='%s' target='_blank'>Click here to download the file.</a>", $downloadLink);
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

        $retData = array('message' => 'Mail has been sent to the given address.');
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
        $fsize = @filesize($downloadFile);

        $data = file_get_contents($downloadFile); // Read the file's contents
        unlink($downloadFile); // remove the file from the server
        force_download($filename, $data);
    }

    ## functions for reports

    function calc_width_percent($total, $width) {
        $ret = ($width / $total) * 100;
        return $ret;
    }

    function changeDateFields($data, $index) {
        foreach ($data as &$row) {
            $row[$index] = getDateStatus($row[$index], false, true);
        }
        return $data;
    }

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

    function rpt_bundle_profile($hostkey, $search, $class_regex, $rows=0, $page_number=0) {

        $header = array('Host', 'Bundle', 'Last verified', 'Hours Ago', 'Avg interval', 'Uncertainty');
        $ret = cfpr_report_bundlesseen($hostkey, $search, true, $class_regex, $rows, $page_number);
        $jsondata = json_decode($ret, true);
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

    function rpt_business_value($hostkey, $days, $months, $years, $class_regex, $rows=0, $page_number=0) {

        $ret = cfpr_report_value($hostkey, $days, $months, $years, $class_regex, $rows, $page_number);
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

    function rpt_class_profile($hostkey, $search, $class_regex, $rows=0, $page_number=0) {

        $header = array('Host', 'Class Context', 'Occurs with probability', 'Uncertainty', 'Last seen');

        $ret = cfpr_report_classes($hostkey, $search, true, $class_regex, $rows, $page_number);
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

    function rpt_promise_notkept($hostkey, $search, $hours_deltafrom, $hours_deltato, $class_regex='', $rows=0, $page_number=0) {

        $header = array('Host', 'Promise Handle', 'Report', 'Time');

        $ret = cfpr_report_notkept($hostkey, $search, intval($hours_deltafrom), intval($hours_deltato), $class_regex, $rows, $page_number);
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

    function rpt_promise_notkept_summary($hostkey, $search, $hours_deltafrom, $hours_deltato, $class_regex='', $rows=0, $page_number=0) {

        $ret = cfpr_summarize_notkept($hostkey, $search, intval($hours_deltafrom), intval($hours_deltato), $class_regex, $rows, $page_number);
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

    function rpt_promise_repaired_summary($hostkey, $search, $hours_deltafrom, $hours_deltato, $class_regex='', $rows=0, $page_number=0) {
        $header = array('Promise Handle', 'Report', 'Occurrences');

        $ret = cfpr_summarize_repaired($hostkey, $search, intval($hours_deltafrom), intval($hours_deltato), $class_regex, $rows, $page_number);

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

    function rpt_compliance_promises($hostkey, $search, $state, $class_regex, $rows=0, $page_number=0) {
        $header = array('Host', 'Promise Handle', 'Last known state', 'Probability kept', 'Uncertainty', 'Last seen');

        $ret = cfpr_report_compliance_promises($hostkey, $search, $state, true, $class_regex, $rows, $page_number);
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

    function rpt_compliance_summary($hostkey, $class_regex, $rows=0, $page_number=0) {
        $header = array('Host', 'Policy', 'Kept', 'Repaired', 'Not kept', 'Last seen');


        $ret = cfpr_report_compliance_summary($hostkey, NULL, -1, -1, -1, -1, ">", $class_regex, $rows, $page_number);
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

    function rpt_filechange_log($hostkey, $search, $class_regex, $longterm_data, $rows=0, $page_number=0) {
        $cols = 3;
        $col_len = array(33, 34, 33);
        $header = array('Host', 'File', 'Time of Change');

        if ($longterm_data) {
            $data['report_result'] = cfpr_report_filechanges_longterm($hostkey, $search, true, -1, ">", $class_regex, $rows, $page_number);
        } else {
            $data['report_result'] = cfpr_report_filechanges($hostkey, $search, true, -1, ">", $class_regex, $rows, $page_number);
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

    function rpt_lastsaw_hosts($hostkey, $key, $name, $address, $ago, $class_regex, $rows=0, $page_number=0) {
        $header = array('Host', 'Initiated', 'IP Address', 'Remote Host', 'Last Seen', 'Hours Ago', ' Avg Interval', 'Uncertainty', 'Remote Host Key');

        $ret = cfpr_report_lastseen($hostkey, $key, $name, $address, $ago, true, $class_regex, $rows, $page_number);
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

    function rpt_patches_available($hostkey, $search, $version, $arch, $class_regex, $rows=0, $page_number=0) {

        $ret = cfpr_report_patch_avail($hostkey, $search, $version, $arch, true, $class_regex, $rows, $page_number);
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

    function rpt_patch_status($hostkey, $search, $version, $arch, $class_regex, $rows=0, $page_number=0) {

        $ret = cfpr_report_patch_in($hostkey, $search, $version, $arch, true, $class_regex, $rows, $page_number);
        $jsondata = json_decode($ret, true);

        $data1 = $jsondata['data'];
        $header = $jsondata['meta']['header'];

        $header = array_keys($header);
        $data1 = $this->changeDateFields($data1, 4);

        $cols = count($header);
        $col_len = array(30, 30, 10, 10,20);


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

    function rpt_software_installed($hostkey, $search, $version, $arch, $class_regex, $rows=0, $page_number=0) {

        $ret = cfpr_report_software_in($hostkey, $search, $version, $arch, true, $class_regex, $rows, $page_number);
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

    function rpt_performance($hostkey, $search, $class_regex, $rows=0, $page_number=0) {

        $ret = cfpr_report_performance($hostkey, $search, true, $class_regex, $rows, $page_number);
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

    function rpt_repaired_log($hostkey, $search, $hours_deltafrom, $hours_deltato, $class_regex, $rows=0, $page_number=0) {
        $header = array('Host', 'Promise Handle', 'Report', 'Time');

        $ret = cfpr_report_repaired($hostkey, $search, intval($hours_deltafrom), intval($hours_deltato), $class_regex, $rows, $page_number);
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

    function rpt_virtualBundles($name, $allUser, $page=0, $rows=0) {
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

    function rpt_variables($hostkey, $search, $scope, $lval, $rval, $type, $class_regex, $rows=0, $page_number=0) {
        $header = array();
        
        if ($hostkey == NULL) {
            $ret = cfpr_report_vars(NULL, $scope, $lval, $rval, $type, true, $class_regex, $rows, $page_number);
        } else {
            $ret = cfpr_report_vars($hostkey, NULL, $search, NULL, NULL, true, $class_regex, $rows, $page_number);
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

    function rpt_filediffs($hostkey, $search, $diff, $cal, $class_regex, $longterm, $page=0, $rows=0) {

        if ($longterm) {
            $data['report_result'] = cfpr_report_filediffs_longterm(NULL, $search, $diff, true, $cal, ">", $class_regex, $page, $rows);
        } else {
            $data['report_result'] = cfpr_report_filediffs(NULL, $search, $diff, true, $cal, ">", $class_regex, $page, $rows);
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

    function rpt_setuid($hostkey, $search, $class_regex, $rows=0, $page_number=0) {
        $header = array('Host', 'Type', 'Name', 'Value');
        $ret = cfpr_report_setuid($hostkey, $search, true, $class_regex, $rows, $page_number);

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