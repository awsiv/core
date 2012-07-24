<?php

class pdfreports extends Cf_Controller
{

    var $reportGenerator = '';
    public $storeDir = './tmp/';
    public $truncateWarning = null;
    public $report_id;
    public $reportTimeZone;

    function __construct()
    {
        parent::__construct();
        $this->load->library('cf_pdf');
        $this->load->library('cf_csv');
        $this->load->model('report_model');
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
            'from',
            'to',
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
            'inclist',
            'exlist',
            'date',
            'cause',
            'var_type',
            'clevel',
            'host_only'
        );
    }

    function index()
    {
        $this->load->library('email');
        $params = $this->uri->uri_to_assoc(3, $this->predefinedKeys);
        $report_type = isset($params['type']) ? urldecode($params['type']) : $this->input->post('type');
        $this->report_id = $report_type;
        $report_format = 'pdf';
        if (isset($params['rf']) && (trim($params['rf']) != ''))
        {
            $report_format = trim($params['rf']);
        }

        $params = array_map('urldecode', $params);

        if ($this->input->post('rf') != null)
        {
            $report_format = $this->input->post('rf');
        }

        $report_download = isset($params['download']) && trim($params['download'] != '') ? $params['download'] : $this->input->post('download');

        // check if its all download or current page download only
        if ($report_download == 1)
        {
            // it all download so pagination paramteres are null
            $params['page'] = null;
            $params['rows'] = null;
        }
        else
        {
            // check the current page and rows
            $params['page'] = $this->input->post('page');
            $params['rows'] = $this->input->post('rows');
        }
        
        $timezone=isset($params['timezone']) && trim($params['timezone'] != '') ? $params['timezone'] : $this->input->post('timezone');
        $this->reportTimeZone=$timezone;

        if (trim($report_format) === 'pdf')
        {
            $filename = 'CFEngine_Enterprise_' . preg_replace('/ /', '_', $report_type) . "-" . date('m-d-Y-His') . '.pdf';

            $pdf = $this->cf_pdf;
            try
            {
                $pdf->PDFSetReportName($this->report_model->getReportTitle($report_type));
            }
            catch (Exception $e)
            {
                $this->output->set_status_header('400', $e->getMessage());
                echo $e->getMessage();
                exit();
            }
            $pdf->PDFSetTableTitle('DATA REPORTED');
            $pdf->AliasNbPages();
            $pdf->SetFont('Arial', '', 14);
            $pdf->AddPage();
            $this->reportGenerator = $pdf;
        }
        else if (trim($report_format) === 'csv')
        {
            $filename = 'CFEngine_Enterprise_' . preg_replace('/ /', '_', $report_type) . "-" . date('m-d-Y-His') . '.csv';
            $this->cf_csv->setFileName($filename);
            $this->reportGenerator = $this->cf_csv;
        }
        // we will use username for RBAC
        $username = $this->session->userdata('username');

        switch ($report_type)
        {
            case "bundle-profile":
                $this->rpt_bundle_profile($username, $params['hostkey'], $params['search'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['clevel'], $params['host_only']);
                break;

            case "business-value":
                $this->rpt_business_value($username, $params['hostkey'], $params['date'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "contexts":
                $this->rpt_class_profile($username, $params['hostkey'], $params['search'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "promise-compliance":
                $this->rpt_compliance_promises($username, $params['hostkey'], $params['search'], $params['state'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['clevel'], $params['host_only']);
                break;

            case "compliance-summary":
                $this->rpt_compliance_summary($username, $params['hostkey'], $params['version'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "file-change-log":
                $this->rpt_filechange_log($username, $params['hostkey'], $params['search'], $params['from'], $params['to'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "neighbors":
                $this->rpt_lastsaw_hosts($username, $params['hostkey'], $params['key'], $params['search'], $params['address'], $params['ago'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "patches-available":
                $this->rpt_patches_available($username, $params['hostkey'], $params['search'], $params['version'], $params['arch'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "patches-installed":
                $this->rpt_patch_status($username, $params['hostkey'], $params['search'], $params['version'], $params['arch'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "benchmarks":
                $this->rpt_performance($username, $params['hostkey'], $params['search'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "promises-repaired-summary":
                $this->rpt_promise_repaired_summary($username, $params['hostkey'], $params['search'], $params['cause'], $params['from'], $params['to'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "promises-repaired-log":
                $this->rpt_repaired_log($username, $params['hostkey'], $params['search'], $params['cause'], $params['from'], $params['to'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "promises-not-kept-summary":
                $this->rpt_promise_notkept_summary($username, $params['hostkey'], $params['search'], $params['cause'], $params['from'], $params['to'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "promises-not-kept-log":
                $this->rpt_promise_notkept($username, $params['hostkey'], $params['search'], $params['cause'], $params['from'], $params['to'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "setuid-programs":
                $this->rpt_setuid($username, $params['hostkey'], $params['search'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "software-installed":
                $this->rpt_software_installed($username, $params['hostkey'], $params['search'], $params['version'], $params['arch'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "values":
                $this->rpt_variables($username, $params['hostkey'], $params['search'], $params['scope'], $params['lval'], $params['rval'], $params['var_type'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "file-change-diffs":
                $this->rpt_filediffs($username, $params['hostkey'], $params['search'], $params['diff'], $params['from'], $params['to'], explode(',', $params['inclist']), explode(',', $params['exlist']), $params['rows'], $params['page'], $params['host_only']);
                break;

            case "virtual-bundles":
                $name = isset($params['name']) ? $params['name'] : null;
                $allUser = isset($params['all_user']) ? $params['all_user'] : null;
                $this->rpt_virtualBundles($name, $allUser, $params['rows'], $params['page']);
                break;
        }

        $pdf_action = key_exists('pdfaction', $params) ? $params['pdfaction'] : $this->input->post('pdfaction');
        # get email parameters from ajax query
        if ($pdf_action == 'email')
        {
            try
            {
                $to      = $this->input->post('to');
                $from    = $this->input->post('from');
                $subject = $this->input->post('subject'); 
                $msg     = $this->input->post('message'); 
                // write the file
                // check for directory
                $this->checkTempDir();
                $filename = $this->storeDir . $filename;
                $doc = $this->reportGenerator->Output($filename, 'F');
                if (trim($to) === '')
                {
                    throw new Exception('Email address is empty.');
                }
                $this->emailReport($filename, $to, $from, $subject, $msg);
            }
            catch (Exception $e)
            {
                $this->output->set_status_header('400', $e->getMessage());
                echo $e->getMessage();
                exit();
            }
        }
        else
        {
            try
            {
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
            }
            catch (Exception $e)
            {
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
    function checkTempDir()
    {
        if (!file_exists($this->storeDir))
        {
            if (!mkdir($this->storeDir, 0777))
            {
                log_message('error', 'Please make sure the tmp directory in web root is present');
                throw new Exception('"tmp" directory in web root not found.');
            }
        }
        if (!is_writable($this->storeDir))
        {
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
    function emailReport($filename, $to, $from, $subject, $message)
    {

        // attachment name
        // encode data (puts attachment in proper format)

        $this->email->from($from);
        $this->email->to($to);

        $this->email->subject($subject);
        $this->email->message($message);
        $this->email->attach($filename);

        if (!$this->email->send())
        {
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
    function download()
    {
        $this->load->helper('download');
        $params = $this->uri->uri_to_assoc(3);
        $filename = isset($params['file']) ? urldecode($params['file']) : null;
        if (is_null($filename))
        {
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
    function checkForDataTruncation($result)
    {
        if (is_array($result) && key_exists('truncated', $result['meta']))
        {
            $this->truncateWarning = $result['meta']['truncated'];
        }
    }

    /**
     * Changes the timestamp to date format
     * @param array $data
     * @param type $index array index of the timestamp field
     */
    function changeDateFields(&$data, $index)
    {
        $count = count($data);
        for ($i = 0; $i < $count; $i++)
        {
            $data[$i][$index] = getDateStatus($data[$i][$index], false, true,$this->reportTimeZone);
        }
    }

    private function conversionForHostOnlyData($data)
    {
        $result = array();
        foreach ($data['data'] as $row)
        {
            $result[] = array($row['hostname'], $row['ip'], $row['hostkey']);
        }
        return $result;
    }

    function rpt_bundle_profile($username, $hostkey, $search, $inclist, $exlist, $rows = 0, $page_number = 0, $clevel = false, $host_only = false)
    {

        $header = array('Host', 'Bundle', 'Last verified', 'Hours Ago', 'Avg interval', 'Uncertainty');
        try
        {
            if ($host_only)
            {
                $ret = $this->report_model->getHostWithBundles($username, $search, $inclist, $exlist, $rows, $page_number, $clevel);
                $header = array('Host name', 'ip', 'key');
                $data1 = $this->conversionForHostOnlyData($ret);
                $this->checkForDataTruncation($ret);
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {

                $ret = $this->report_model->getBundleReport($username, $hostkey, $search, $inclist, $exlist, $rows, $page_number, $clevel);
                $this->checkForDataTruncation($ret);
                $data1 = $ret['data'];
                $header = ($ret['meta']['header']);

                $header = array_keys($header);
                $this->changeDateFields($data1, 2);
                $cols = count($header);
                $col_len = array(24, 23, 23, 10, 10, 10); #in percentage
            }

            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);

                $pdf->PDFSetDescription($desc);

                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);

                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1, 'bundle_profile_report.csv');
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_business_value($username, $hostkey, $date, $inclist, $exlist, $rows = 0, $page_number = 0, $hosts_only = false)
    {
        try
        {

            if ($hosts_only)
            {
                $jsondata = $this->report_model->getHostsWithBusinessValue($username, $date, $inclist, $exlist, $rows, $page_number);
                $this->checkForDataTruncation($jsondata);
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $jsondata = $this->report_model->getBusinessValueReport($username, $hostkey, $date, $inclist, $exlist, $rows, $page_number);
                $this->checkForDataTruncation($jsondata);

                $data1 = $jsondata['data'];
                $header = ($jsondata['meta']['header']);
                $header = array_keys($header);
                $cols = count($header);
                $col_len = array(24, 19, 19, 19, 19);
            }
            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);
                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);

                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_class_profile($username, $hostkey, $search, $inclist, $exlist, $rows = 0, $page_number = 0, $hosts_only = false)
    {

        $header = array('Host', 'Class Context', 'Occurs with probability', 'Uncertainty', 'Last seen');
        try
        {
            $ret = $this->report_model->getClassReport($username, $hostkey, $search, $inclist, $exlist, $rows, $page_number, $hosts_only);
            $this->checkForDataTruncation($ret);
            if ($hosts_only)
            {
                $data1 = $this->conversionForHostOnlyData($ret);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $ret['data'];
                $header = $ret['meta']['header'];

                $header = array_keys($header);
                $this->changeDateFields($data1, 4);
                $cols = count($header);
                $col_len = array(28, 28, 18, 10, 16);
            }

            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);
                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);
                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('500', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_promise_notkept($username, $hostkey, $search, $cause, $from, $to, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {

        try
        {

            $to = (!(int) ($to)) ? time() : $to;
            $from = (!(int) ($from)) ? 0 : $from;

            $jsondata = $this->report_model->getPromisesNotKeptLog($username, $hostkey, $search, $cause, $from, $to, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);

            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];

                unset($header['Note']);
                $header = array_keys($header);
                $this->changeDateFields($data1, 3);
                $col_len = array(15, 21, 49, 15);
            }

            if ($this->reportGenerator instanceof cf_pdf)
            {

                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);

                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);

                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, count($header), $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_promise_notkept_summary($username, $hostkey, $search, $cause, $from, $to, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {
        try
        {

            $to = (!(int) ($to)) ? time() : $to;
            $from = (!(int) ($from)) ? 0 : $from;

            $jsondata = $this->report_model->getPromisesNotKeptSummary($username, $hostkey, $search, $cause, $from, $to, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);

            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];
                $header = array_keys($header);
                $col_len = array(25, 55, 20);
            }

            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);

                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);

                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, count($header), $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_promise_repaired_summary($username, $hostkey, $search, $cause, $from, $to, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {
        $header = array('Promise Handle', 'Report', 'Occurrences');
        try
        {
            $to = (!(int) ($to)) ? time() : $to;
            $from = (!(int) ($from)) ? 0 : $from;

            $jsondata = $this->report_model->getPromisesRepairedSummary($username, $hostkey, $search, $cause, $from, $to, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);
            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];

                $header = array_keys($header);
                $cols = count($header);
                $col_len = array(25, 55, 20);
            }
            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);

                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);

                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_compliance_promises($username, $hostkey, $search, $state, $inclist, $exlist, $rows = 0, $page_number = 0, $clevel = false, $hosts_only = false)
    {
        $header = array('Host', 'Promise Handle', 'Last known state', 'Probability kept', 'Uncertainty', 'Last seen');

        try
        {
            $jsondata = $this->report_model->getPromiseCompliance($username, $hostkey, $search, $state, $inclist, $exlist, $rows, $page_number, $hosts_only, $clevel);
            $this->checkForDataTruncation($jsondata);
            if ($hosts_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {

                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];

                $header = array_keys($header);
                $this->changeDateFields($data1, 5);

                $cols = count($header);
                $col_len = array(21, 24, 14, 14, 12, 15);
            }

            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);

                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);

                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_compliance_summary($username, $hostkey, $version, $incList, $exList, $rows = 0, $page_number = 0, $hosts_only = false)
    {

        try
        {
            $jsondata = $this->report_model->getComplianceSummary($username, $hostkey, $version, $incList, $exList, $rows, $page_number, $hosts_only);
            $this->checkForDataTruncation($jsondata);
            if ($hosts_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];

                $header = array_keys($header);
                $this->changeDateFields($data1, 5);

                $cols = count($header);
                $col_len = array(25, 33, 8, 8, 8, 18);
            }
            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);
                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);

                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Excception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_filechange_log($username, $hostkey, $search, $from, $to, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {

        try
        {
            $jsondata = $this->report_model->getFileChangeLog($username, $hostkey, $search, $from, $to, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);
            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];
                unset($header['Note']);
                $header = array_keys($header);
                $this->changeDateFields($data1, 2);
                $cols = 3;
                $col_len = array(33, 34, 33);
            }


            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);
                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);
                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_lastsaw_hosts($username, $hostkey, $key, $name, $address, $ago, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {
        $header = array('Host', 'Initiated', 'IP Address', 'Remote Host', 'Last Seen', 'Hours Ago', ' Avg Interval', 'Uncertainty', 'Remote Host Key');

        try
        {
            $jsondata = $this->report_model->getLastSeenReport($username, $hostkey, $key, $name, $address, $ago, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);

            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];

                $header = array_keys($header);
                $this->changeDateFields($data1, 4);

                $cols = count($header);
                $col_len = array(14, 8, 10, 14, 12, 7, 7, 7, 21);
            }
            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);

                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);
                $pdf->SetFont('Arial', '', 6);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_patches_available($username, $hostkey, $search, $version, $arch, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {
        try
        {
            $jsondata = $this->report_model->getPatchesAvailable($username, $hostkey, $search, $version, $arch, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);

            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];
                $this->changeDateFields($data1, 4);
                $header = array_keys($header);
                $cols = count($header);
                $col_len = array(30, 30, 10, 10, 20);
            }
            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);

                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);
                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_patch_status($username, $hostkey, $search, $version, $arch, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {

        try
        {
            $jsondata = $this->report_model->getPatchesInstalled($username, $hostkey, $search, $version, $arch, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);
            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];

                $header = array_keys($header);
                $this->changeDateFields($data1, 4);

                $cols = count($header);
                $col_len = array(30, 30, 10, 10, 20);
            }

            if ($this->reportGenerator instanceof cf_pdf)
            {
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf = $this->reportGenerator;
                $pdf->PDFSetDescription($desc);
                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);
                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_software_installed($username, $hostkey, $search, $version, $arch, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {
        try
        {
            $jsondata = $this->report_model->getSoftwareInstalled($username, $hostkey, $search, $version, $arch, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);
            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];
                $header = array_keys($header);
                $this->changeDateFields($data1, 4);
                $cols = count($header);
                $col_len = array(30, 30, 10, 10, 20);
            }
            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);
                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);
                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_performance($username, $hostkey, $search, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {
        try
        {
            $jsondata = $this->report_model->getPerformance($username, $hostkey, $search, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);
            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];
                $header = array_keys($header);
                $this->changeDateFields($data1, 5);
                $cols = count($header);
                $col_len = array(19, 38, 7.5, 7.5, 10, 18);
            }

            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);

                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);
                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_repaired_log($username, $hostkey, $search, $cause, $from, $to, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {
        try
        {
            $to = (!(int) $to) ? time() : $to;
            $from = (!(int) $from) ? 0 : $from;
            $jsondata = $this->report_model->getPromisesRepairedLog($username, $hostkey, $search, $cause, $from, $to, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);
            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {
                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];
                $header = array_keys($header);
                $this->changeDateFields($data1, 3);
                $cols = count($header);
                $col_len = array(19, 19, 43, 19);
            }

            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);

                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);
                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_virtualBundles($name, $allUser, $page = 0, $rows = 0)
    {
        $this->load->model('virtual_bundle_model');
        $cols = 4;
        $col_len = array(19, 19, 43, 19);
        $header = array('Virtual bundle', 'Promises', 'Hosts', 'Compliance');

        try
        {
            $ret = $this->virtual_bundle_model->getVirtualBundleData($name, $allUser);
            $dataArray = json_decode($ret, true);
            $data1 = $dataArray['data'];

            if ($this->reportGenerator instanceof cf_pdf)
            {

                $pdf = $this->reportGenerator;
                $desc = cfpr_report_description('Virtual bundles');
                $pdf->PDFSetDescription($desc);
                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);
                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_variables($username, $hostkey, $search, $scope, $lval, $rval, $type, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {
        $header = array();

        $ret = $this->report_model->getVariablesReport($username, $hostkey, $scope, $lval, $rval, $type, $inclist, $exlist, $rows, $page_number, $host_only);
        $this->checkForDataTruncation($ret);

        if ($host_only)
        {
            $data1 = $this->conversionForHostOnlyData($ret);
            $header = array('Host name', 'ip', 'key');
            $cols = count($header);
            $col_len = array(30, 25, 45); #in percentage
        }
        else
        {
            $dataArray = $ret;
            $data1 = $dataArray;

            foreach ($data1 as $topic => &$subtopic)
            {
                if ($topic == 'meta')
                    continue;
                $head = array_keys($subtopic['header']);
                $newArray[][] = $topic;
                $newArray[] = $head;
                // change the time stamp
                $this->changeDateFields($subtopic['data'], 4);

                $newArray = array_merge($newArray, $subtopic['data']);
            }

            $cols = 5;
            $col_len = array(20, 20, 20, 20, 20);
        }

        if ($this->reportGenerator instanceof cf_pdf)
        {
            $pdf = $this->reportGenerator;
            $desc = $this->report_model->getReportDescription($this->report_id);
            $pdf->PDFSetDescription($desc);

            $pdf->ReportTitle();
            $pdf->ReportDescription();
            $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
            $pdf->Ln(8);

            $pdf->SetFont('Arial', '', 9);
            if ($host_only)
            {
                $pdf->DrawTable($data1, count($header), $col_len, $header, 8);
            }
            else
            {
                $pdf->drawMultiTable($data1, $col_len);
            }
        }
        else if ($this->reportGenerator instanceof cf_csv)
        {
            $this->cf_csv->initialize($header, $newArray);
        }
    }

    function rpt_filediffs($username, $hostkey, $search, $diff, $from, $to, $inclist, $exlist, $rows, $page_number, $host_only = false)
    {

        try
        {
            $result = $this->report_model->getFileChangeDiff($username, $hostkey, $search, $diff, $from, $to, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($result);

            if ($host_only)
            {
                $newFormat = $this->conversionForHostOnlyData($result);
                $header = array('Host name', 'ip', 'key');
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {

                $newFormat = array();
                foreach ($result['data'] as $parentIndex => $data)
                {

                    foreach ($data[3] as $index => $subArray)
                    {
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
                $col_len = array(20, 15, 15, 10, 10, 30);
            }
            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);

                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);

                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($newFormat, count($header), $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $newFormat);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

    function rpt_setuid($username, $hostkey, $search, $inclist, $exlist, $rows = 0, $page_number = 0, $host_only = false)
    {
        try
        {
            $jsondata = $this->report_model->getReportSetUid($username, $hostkey, $search, $inclist, $exlist, $rows, $page_number, $host_only);
            $this->checkForDataTruncation($jsondata);

            if ($host_only)
            {
                $data1 = $this->conversionForHostOnlyData($jsondata);
                $header = array('Host name', 'ip', 'key');
                $cols = count($header);
                $col_len = array(30, 25, 45); #in percentage
            }
            else
            {

                $data1 = $jsondata['data'];
                $header = $jsondata['meta']['header'];
                $header = array_keys($header);
                $cols = count($header);
                $col_len = array(30, 70);
            }

            if ($this->reportGenerator instanceof cf_pdf)
            {
                $pdf = $this->reportGenerator;
                $desc = $this->report_model->getReportDescription($this->report_id);
                $pdf->PDFSetDescription($desc);
                $pdf->ReportTitle();
                $pdf->ReportDescription();
                $pdf->RptTableTitle($pdf->tabletitle, $pdf->GetY() + 5);
                $pdf->Ln(8);

                $pdf->SetFont('Arial', '', 9);
                $pdf->DrawTable($data1, $cols, $col_len, $header, 8);
            }
            else if ($this->reportGenerator instanceof cf_csv)
            {
                $this->cf_csv->initialize($header, $data1);
            }
        }
        catch (Exception $e)
        {
            $this->output->set_status_header('400', $e->getMessage());
            echo $e->getMessage();
            exit();
        }
    }

}

