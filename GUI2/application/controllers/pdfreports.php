<?php

class pdfreports extends Cf_Controller {

    function __construct() {
        parent::__construct();
        $this->load->library('cf_pdf');
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
            'pdfaction',
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
            'pdfaction',
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

        //$params = $this->populateParamsWithDefault($params);
        $report_type = isset($params['type']) ? $params['type'] : "";
        $pdf_filename = 'Nova_' . preg_replace('/ /', '_', $report_type) . '.pdf';
        $pdf = new cf_pdf();
        $pdf->PDFSetReportName($report_type);
        $pdf->PDFSetTableTitle('DATA REPORTED');
        $pdf->AliasNbPages();
        $pdf->SetFont('Arial', '', 14);
        $pdf->AddPage();

        switch ($report_type) {
            case "Bundle profile":
                $desc = cfpr_report_description('bundle profile');
                $pdf->PDFSetDescription($desc);
                $this->rpt_bundle_profile($params['hostkey'], $params['search'], $pdf, $params['class_regex']);
                break;

            case "Business value report":
                $desc = cfpr_report_description('value report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_business_value($params['hostkey'], $params['days'], $params['months'], $params['years'], $pdf, $params['class_regex']);
                break;

            case "Class profile":
                $desc = cfpr_report_description('classes report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_class_profile($params['hostkey'], $params['search'], $pdf, $params['class_regex']);
                break;

            case "Compliance by promise":
                $desc = cfpr_report_description('promise report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_compliance_promises($params['hostkey'], $params['search'], $params['state'], $pdf, $params['class_regex']);
                break;

            case "Compliance summary":
                $desc = cfpr_report_description('compliance report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_compliance_summary($params['hostkey'], $pdf, $params['class_regex']);
                break;

            case "File change log":
                $desc = cfpr_report_description('file_changes report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_filechange_log($params['hostkey'], $params['search'], $pdf, $params['class_regex']);
                break;

            case "Last saw hosts":
                $desc = cfpr_report_description('lastseen report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_lastsaw_hosts($params['hostkey'], $params['key'], $params['search'], $params['address'], $params['ago'], $pdf, $params['class_regex']);
                break;

            case "Patches available":
                $desc = cfpr_report_description('patches available report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_patches_available($params['hostkey'], $params['search'], $params['version'], $params['arch'], $pdf, $params['class_regex']);
                break;

            case "Patch status":
                $desc = cfpr_report_description('patches installed report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_patch_status($params['hostkey'], $params['search'], $params['version'], $params['arch'], $pdf, $params['class_regex']);
                break;

            case "Performance":
                $desc = cfpr_report_description('performance report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_performance($params['hostkey'], $params['search'], $pdf, $params['class_regex']);
                break;

            case "Promises repaired summary":
                $desc = cfpr_report_description('promises repaired summary');
                $pdf->PDFSetDescription($desc);
                $this->rpt_repaired_log($params['hostkey'], $params['search'], $pdf, $params['class_regex']);
                break;

            case "Promises repaired log":
                $desc = cfpr_report_description('promises repaired report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_repaired_log($params['hostkey'], $params['search'], $pdf, $params['hours_deltafrom'], $params['hours_deltato'], $params['class_regex']);
                break;

            case "Promises not kept summary":
                $desc = cfpr_report_description('promises repaired report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_promise_notkept($params['hostkey'], $params['search'], $pdf, $params['class_regex']);
                break;

            case "Promises not kept log":
                $desc = cfpr_report_description('promises not kept report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_promise_notkept($params['hostkey'], $params['search'], $pdf, $params['hours_deltafrom'], $params['hours_deltato'], $params['class_regex']);
                break;

            case "Setuid/gid root programs":
                $desc = cfpr_report_description('setuid report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_setuid($params['hostkey'], $params['search'], $pdf, $params['class_regex']);
                break;

            case "Software installed":

                $desc = cfpr_report_description('software installed report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_software_installed($params['hostkey'], $params['search'], $params['version'], $params['arch'], $pdf, $params['class_regex']);
                break;

            case "Variables":
                $desc = cfpr_report_description('variables report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_variables($params['hostkey'], $params['search'], $params['scope'], $params['lval'], $params['rval'], $params['var_type'], $pdf, $params['class_regex']);
                break;

            case "File change diffs":
                $desc = cfpr_report_description('file_diffs report');
                $pdf->PDFSetDescription($desc);
                $this->rpt_filediffs($params['hostkey'], $params['search'], $params['diff'], $params['cal'], $pdf, $params['class_regex']);
                break;
        }

        $pdf_action = $params['pdfaction'];

        # get email parameters from ajax query
        if ($pdf_action == 'email') {

            $to = $_POST['to'];
            $from = $_POST['from'];
            $subject = $_POST['subject'];
            $msg = $_POST['message'];
            $this->emailPDF($pdf, $pdf_filename, $to, $from, $subject, $msg);
        } else {
            $pdf->Output($pdf_filename, "D");
        }
    }

    /**
     * Emails the pdf
     * @param <type> $pdf
     * @param <type> $pdf_filename
     * @param <type> $to
     * @param <type> $from
     * @param <type> $subject
     * @param <type> $message
     */
    function emailPDF($pdf, $pdf_filename, $to, $from, $subject, $message) {

        // a random hash will be necessary to send mixed content
        $separator = md5(time());

        // carriage return type (we use a PHP end of line constant)
        $eol = PHP_EOL;

        // attachment name
        $filename = $pdf_filename;
        // encode data (puts attachment in proper format)
        $pdfdoc = $pdf->Output("./tmp/$filename", "F");

        $this->email->from($from);
        $this->email->to($to);

        $this->email->subject($subject);
        $this->email->message('Email from cfengine.');
        $this->email->attach('./tmp/' . $filename);

        if (!$this->email->send()) {
            // error sending mail
            // SOMETHING WENT WRONG WHILE ADDITION
            $this->output->set_status_header('400', 'Cannot send the mail this time.');
            echo 'Something went wrong while sending mail';
            exit;
        }

        $retData = array('message' => 'Mail has been sent to the given address.');
        $jsonReturn = json_encode($retData);

        echo $jsonReturn;
        @unlink('./tmp/' . $filename);
        exit();
    }

    ## functions for reports

    function calc_width_percent($total, $width) {
        $ret = ($width / $total) * 100;
        return $ret;
    }

    function rpt_bundle_profile($hostkey, $search, &$pdf, $class_regex) {
        $cols = 6;
        $col_len = array(24, 23, 23, 10, 10, 10); #in percentage
        $header = array('Host', 'Bundle', 'Last verified', 'Hours Ago', 'Avg interval', 'Uncertainty');

        $ret = cfpr_report_bundlesseen_pdf($hostkey, $search, true, $class_regex);
        $data1 = $pdf->ParseData($ret);

        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);

        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

### business value report ###

    function rpt_business_value($hostkey, $days, $months, $years, &$pdf, $class_regex) {
        $cols = 5;
        $col_len = array(24, 19, 19, 19, 19);
        $header = array('Host', 'Day', 'Kept', 'Repaired', 'Not kept');

        $ret = cfpr_report_value_pdf($hostkey, $days, $months, $years, $class_regex);
        $data1 = $pdf->ParseData($ret);

        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);

        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

### Classes report ###

    function rpt_class_profile($hostkey, $search, &$pdf, $class_regex) {
        $cols = 5;
        $col_len = array(28, 28, 18, 10, 16);
        $header = array('Host', 'Class Context', 'Occurs with probability', 'Uncertainty', 'Last seen');

        $ret = cfpr_report_classes_pdf($hostkey, $search, true, $class_regex);
        $data1 = $pdf->ParseData($ret);

        # count the number of columns
        #$cols = (count($data1,1)/count($data1,0))-1;

        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);

        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

    function rpt_promise_notkept($hostkey, $search, &$pdf, $hours_deltafrom, $hours_deltato, $class_regex) {
        $col_len = array(20, 20, 40, 20);
        $header = array('Host', 'Promise Handle', 'Report', 'Time');

        $ret = cfpr_report_notkept_pdf($hostkey, $search, intval($hours_deltafrom), intval($hours_deltato), $class_regex);
        $data1 = $pdf->ParseData($ret);

        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);

        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, 4, $col_len, $header, 8);
    }

### Compliance by promise ##

    function rpt_compliance_promises($hostkey, $search, $state, &$pdf, $class_regex) {
        $cols = 6;
        $col_len = array(21, 24, 14, 14, 12, 15);
        $header = array('Host', 'Promise Handle', 'Last known state', 'Probability kept', 'Uncertainty', 'Last seen');

        $ret = cfpr_report_compliance_promises_pdf($hostkey, $search, $state, true, $class_regex);
        $data1 = $pdf->ParseData($ret);

        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);

        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

### Compliance Summary ##

    function rpt_compliance_summary($hostkey, &$pdf, $class_regex) {
        $cols = 6;
        $col_len = array(25, 33, 8, 8, 8, 18);
        $header = array('Host', 'Policy', 'Kept', 'Repaired', 'Not kept', 'Last seen');

        $ret = cfpr_report_compliance_summary_pdf($hostkey, NULL, -1, -1, -1, -1, ">", $class_regex);
        $data1 = $pdf->ParseData($ret);

        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);

        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

### Classes report ###

    function rpt_filechange_log($hostkey, $search, &$pdf, $class_regex) {
        $cols = 3;
        $col_len = array(33, 34, 33);
        $header = array('Host', 'File', 'Time of Change');

        $ret = cfpr_report_filechanges_pdf($hostkey, $search, true, -1, ">", $class_regex);
        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();

        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);
        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

### Classes report ###

    function rpt_lastsaw_hosts($hostkey, $key, $name, $address, $ago, &$pdf, $class_regex) {
        $cols = 9;
        $col_len = array(14, 8, 10, 14, 12, 7, 7, 7, 21);
        $header = array('Host', 'Initiated', 'IP Address', 'Remote Host', 'Last Seen', 'Hours Ago', ' Avg Interval', 'Uncertainty', 'Remote Host Key');

        $ret = cfpr_report_lastseen_pdf($hostkey, $key, $name, $address, $ago, true, $class_regex);
        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);
        $pdf->SetFont('Arial', '', 6);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

### Patches Available  ###

    function rpt_patches_available($hostkey, $search, $version, $arch, &$pdf, $class_regex) {
        $cols = 4;
        $col_len = array(29, 33, 19, 19);
        $header = array('Host', 'Name', 'Version', 'Architecture');

        $ret = cfpr_report_patch_avail_pdf($hostkey, $search, $version, $arch, true, $class_regex);
        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);
        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

### Patch Status  ###

    function rpt_patch_status($hostkey, $search, $version, $arch, &$pdf, $class_regex) {
        $cols = 4;
        $col_len = array(29, 33, 19, 19);
        $header = array('Host', 'Name', 'Version', 'Architecture');

        $ret = cfpr_report_patch_in_pdf($hostkey, $search, $version, $arch, true, $class_regex);
        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);
        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

### Software Installed  ###

    function rpt_software_installed($hostkey, $search, $version, $arch, &$pdf, $class_regex) {
        $cols = 4;
        $col_len = array(21, 33, 19, 19);
        $header = array('Host', 'Name', 'Version', 'Architecture');

        $ret = cfpr_report_software_in_pdf($hostkey, $search, $version, $arch, true, $class_regex);
        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);
        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

### Performance Report  ###

    function rpt_performance($hostkey, $search, &$pdf, $class_regex) {
        $cols = 6;
        $col_len = array(19, 38, 7.5, 7.5, 10, 18);
        $header = array('Host', 'Repair', 'Last Time', 'Avg Time', 'Uncertainty', 'Last Performed');

        $ret = cfpr_report_performance_pdf($hostkey, $search, true, $class_regex);
        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);
        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

    function rpt_repaired_log($hostkey, $search, &$pdf, $hours_deltafrom, $hours_deltato, $class_regex) {
        $cols = 4;
        $col_len = array(19, 19, 43, 19);
        $header = array('Host', 'Promise Handle', 'Report', 'Time');

        $ret = cfpr_report_repaired_pdf($hostkey, $search, intval($hours_deltafrom), intval($hours_deltato), $class_regex);
        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);
        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

    function rpt_repaired_summary($hostkey, $search, &$pdf, $class_regex) {
        $cols = 4;
        $col_len = array(19, 19, 43, 19);
        $header = array('Host', 'Promise Handle', 'Report', 'Occurrences');

        $ret = cfpr_summarize_repaired_pdf($hostkey, $search, $class_regex);
        $data1 = $pdf->ParseData($ret);

        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);

        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

    function rpt_notkept_summary($hostkey, $search, &$pdf, $class_regex) {
        $cols = 4;
        $col_len = array(19, 19, 43, 19);
        $header = array('Host', 'Promise Handle', 'Report', 'Occurrences');

        $ret = cfpr_summarize_notkept_pdf($hostkey, $search, $class_regex);
        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);
        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
    }

    function rpt_variables($hostkey, $search, $scope, $lval, $rval, $type, &$pdf, $class_regex) {
        $cols = 4;
        $col_len = array(19, 15, 19, 47);
        $header = array('Host', 'Type', 'Name', 'Value');

        if ($hostkey == NULL) {
            $ret = cfpr_report_vars_pdf(NULL, $scope, $lval, $rval, $type, true, $class_regex);
        } else {
            $ret = cfpr_report_vars_pdf($hostkey, NULL, $search, NULL, NULL, true, $class_regex);
        }
        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);

        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTableSpecial($data1, $cols, $col_len, $header, 8);
    }

    function rpt_filediffs($hostkey, $search, $diff, $cal, &$pdf, $class_regex) {
        $cols = 4;
        $col_len = array(19, 15, 19, 47);
        $header = array('Host', 'File', 'Change detected at', 'Change');

        $ret = cfpr_report_filediffs_pdf($hostkey, $search, $diff, true, $cal, ">", $class_regex);

        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);

        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTableSpecial($data1, $cols, $col_len, $header, 8);
    }

    function rpt_setuid($hostkey, $search, &$pdf, $class_regex) {
        $cols = 2;
        $col_len = array(30, 70);
        #        $header=array('Host','Type','Name','Value');

        $ret = cfpr_report_setuid_pdf($hostkey, $search, true, $class_regex);

        $data1 = $pdf->ParseData($ret);
        $pdf->ReportTitle();
        $pdf->ReportDescription();
        $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
        $pdf->Ln(8);

        $pdf->SetFont('Arial', '', 9);
        $pdf->DrawTableSpecial($data1, $cols, $col_len, $header, 8);
    }

}