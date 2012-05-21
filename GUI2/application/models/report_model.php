<?php

class report_model extends Cf_Model
{

    /**
     *
     * @return type
     */
    function getAllReports()
    {
        try
        {
            $rawdata = cfpr_select_reports(null);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $report_id
     * @return type
     */
    function getReportCategory($report_id = NULL)
    {
        try
        {
            if ($report_id == NULL)
            {
                throw new Exception($this->lang->line('invalid_report_id'));
            }

            $rawdata = cfpr_select_reports($report_id);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data['data'][0]['category'];
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $report_id
     * @return type
     */
    function getReportTitle($report_id = NUll)
    {
        try
        {
            if ($report_id == NULL)
            {
                throw new Exception($this->lang->line('invalid_report_id'));
            }

            $rawdata = cfpr_select_reports($report_id);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data['data'][0]['name'];
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $report_id
     * @return type
     */
    function getReportDescription($report_id = NULL)
    {
        try
        {
            if ($report_id == NULL)
            {
                throw new Exception($this->lang->line('invalid_report_id'));
            }

            $rawdata = cfpr_select_reports($report_id);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {

                return $data['data'][0]['description'];
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $report_id
     * @return type
     */
    function getReportDetail($report_id = NULL)
    {
        try
        {
            if ($report_id == NULL)
            {
                throw new Exception($this->lang->line('invalid_report_id'));
            }
            $rawdata = cfpr_select_reports($report_id);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data['data'][0];
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type
     */
    function getBundleReport($username, $hostkey, $search, $inclist, $exlist, $rows = 50, $page_number = 1, $complianceLevel=false)
    {
        try
        {
            if ($complianceLevel !== false && $complianceLevel != null)
            {
                $rawdata = cfpr_report_lastknown_bundlesseen($username, $hostkey, $complianceLevel, $search, true, $inclist, $exlist,"last-verified", true, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_bundlesseen($username, $hostkey, $search, true, $inclist, $exlist, "last-verified", true, $rows, $page_number);
            }
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $search
     * @param type $class_regx
     * @return type
     */
    function getHostWithBundles($username, $name, $inclist, $exlist, $rows, $page_number, $complianceLevel=false)
    {

        try
        {
            if ($complianceLevel !== false && $complianceLevel != null)
            {
                $rawdata = cfpr_hosts_with_lastknown_bundlesseen($username, NULL, $complianceLevel, $name, true, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_hosts_with_bundlesseen($username, NULL, $name, true, $inclist, $exlist, $rows, $page_number);
            }
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getClassReport($username, $hostkey, $search, $inclist, $exlist, $rows = 50, $page_number = 1, $host_only = false)
    {
        try
        {
            if ($host_only)
            {
                $rawdata = cfpr_hosts_with_classes($username, NULL, $search, true, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_classes($username, $hostkey, $search, true, $inclist, $exlist, "last-seen", true, $rows, $page_number);
            }
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $days
     * @param type $months
     * @param type $years
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getBusinessValueReport($username, $hostkey, $date, $inclist, $exlist, $rows = 50, $page_number = 1)
    {
        try
        {
            $rawdata = cfpr_report_value($username, $hostkey, $date, NULL, NULL, $inclist, $exlist, "day", true, $rows, $page_number);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $days
     * @param type $months
     * @param type $years
     * @param type $class_regex
     * @return type array
     */
    function getHostsWithBusinessValue($username, $date, $inclist, $exlist, $rows, $page_number)
    {
        try
        {
            $rawdata = cfpr_hosts_with_value($username, NULL, $date, NULL, NULL, $inclist, $exlist, $rows, $page_number);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $state
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getPromiseCompliance($username, $hostkey, $search, $state, $inclist, $exlist, $rows = 50, $page_number = 1, $host_only = false, $complianceLevel=false)
    {
        try
        {
            if ($host_only)
            {

                if ($complianceLevel !== false && $complianceLevel != null)
                {
                    $rawdata = cfpr_hosts_with_lastknown_compliance_promises($username, NULL, $complianceLevel, $search, $state, true, $inclist, $exlist, $rows, $page_number);
                }
                else
                {
                    $rawdata = cfpr_hosts_with_compliance_promises($username, NULL, $search, $state, true, $inclist, $exlist, $rows, $page_number);
                }
            }
            else
            {
                if ($complianceLevel !== false && $complianceLevel != null)
                {
                    $rawdata = cfpr_report_lastknown_compliance_promises($username, $hostkey,$complianceLevel,$search, $state, true, $inclist, $exlist, "last-seen", true, $rows, $page_number);
                }
                else
                {
                    $rawdata = cfpr_report_compliance_promises($username, $hostkey, $search, $state, true, $inclist, $exlist, "last-seen", true, $rows, $page_number);
                }
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getComplianceSummary($username, $hostkey, $version, $inclist, $exlist, $rows = 50, $page_number = 1, $host_only = false)
    {
        try
        {
            if ($host_only)
            {
                $rawdata = cfpr_hosts_with_compliance_summary($username, $hostkey, $version, -1, -1, -1, -1, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_compliance_summary($username, $hostkey, $version, -1, -1, -1, -1, $inclist, $exlist, "last-seen", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $from
     * @param type $to
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type
     */
    function getFileChangeLog($username, $hostkey, $search, $from, $to, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_filechanges($username, NULL, $search, true, $from, $to, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_filechanges($username, $hostkey, $search, true, $from, $to, $inclist, $exlist, "time", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $diff
     * @param type $from
     * @param type $to
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getFileChangeDiff($username, $hostkey, $search, $diff, $from, $to, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_filediffs($username, NULL, $search, $diff, true, $from, $to, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_filediffs($username, NULL, $search, $diff, true, $from, $to, $inclist, $exlist, "time", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    function getLastSeenReport($username, $hostkey, $key, $name, $address, $ago, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_lastseen($username, NULL, $key, $name, $address, $ago, true, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_lastseen($username, $hostkey, $key, $name, $address, $ago, true, $inclist, $exlist, "last-seen", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    function getPatchesAvailable($username, $hostkey, $search, $version, $arch, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_patch_avail($username, NULL, $search, $version, $arch, true, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_patch_avail($username, $hostkey, $search, $version, $arch, true, $inclist, $exlist, "hostname", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $version
     * @param type $arch
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getPatchesInstalled($username, $hostkey, $search, $version, $arch, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_patch_in($username, NULL, $search, $version, $arch, true, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_patch_in($username, $hostkey, $search, $version, $arch, true, $inclist, $exlist, "hostname", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $class_regex
     * @param type $rows
     * @param type $page_number
     */
    function getPerformance($username, $hostkey, $search, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_performance($username, NULL, $search, true, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_performance($username, $hostkey, $search, true, $inclist, $exlist, "last-performed", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $from
     * @param type $to
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getPromisesRepairedLog($username, $hostkey, $search, $cause_rx, $from, $to, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if (!$to)
            {
                $to = time();
            }

            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_repaired($username, NULL, $search, $cause_rx, $from, $to, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_repaired($username, $hostkey, $search, $cause_rx, $from, $to, $inclist, $exlist, "time", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $cause_rx
     * @param type $from
     * @param type $to
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @param type $hosts_only
     * @return type
     */
    function getPromisesRepairedSummary($username, $hostkey, $search, $cause_rx, $from, $to, $inclist = array(), $exlist = array(), $rows = 0, $page_number = 0, $hosts_only = false)
    {
        try
        {
            if (!$to)
            {
                $to = time();
            }

            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_repaired($username, NULL, $search, $cause_rx, $from, $to, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_summarize_repaired($username, $hostkey, $search, $cause_rx, $from, $to, $inclist, $exlist, "time", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $cause_rx
     * @param type $from
     * @param type $to
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @param type $hosts_only
     * @return type
     */
    function getPromisesNotKeptSummary($username, $hostkey, $search, $cause_rx, $from, $to, $inclist = array(), $exlist = array(), $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if (!$to)
            {
                $to = time();
            }

            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_notkept($username, NULL, $search, $cause_rx, $from, $to, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_summarize_notkept($username, $hostkey, $search, $cause_rx, $from, $to, $inclist, $exlist, "time", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $from
     * @param type $to
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getPromisesNotKeptLog($username, $hostkey, $search, $cause_rx, $from, $to, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if (!$to)
            {
                $to = time();
            }

            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_notkept($username, NULL, $search, $cause_rx, $from, $to, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_notkept($username, $hostkey, $search, $cause_rx, $from, $to, $inclist, $exlist, "time", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getReportSetUid($username, $hostkey, $search, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_setuid($username, NULL, $search, true, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_setuid($username, $hostkey, $search, true, $inclist, $exlist, "hostname", true, $rows, $page_number);
            }

            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {

                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $search
     * @param type $version
     * @param type $arch
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @return type array
     */
    function getSoftwareInstalled($username, $hostkey, $search, $version, $arch, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_software_in($username, NULL, $search, $version, $arch, true, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_software_in($username, $hostkey, $search, $version, $arch, true, $inclist, $exlist, "hostname", true, $rows, $page_number);
            }
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $scope
     * @param type $lval
     * @param type $rval
     * @param type $type
     * @param type $inclist
     * @param type $exlist
     * @param type $rows
     * @param type $page_number
     * @param type $hosts_only
     * @return type array
     */
    function getVariablesReport($username, $hostkey, $scope, $lval, $rval, $type, $inclist, $exlist, $rows = 50, $page_number = 1, $hosts_only = false)
    {
        try
        {
            if ($hosts_only)
            {
                $rawdata = cfpr_hosts_with_vars($username, NULL, $scope, $lval, $rval, $type, true, $inclist, $exlist, $rows, $page_number);
            }
            else
            {
                $rawdata = cfpr_report_vars($username, $hostkey, $scope, $lval, $rval, $type, true, $inclist, $exlist, "var-name", true, $rows, $page_number);
            }
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
        
    function getWeakestHosts($username, $includes = array('.*'), $excludes = array(), $rows = 10, $page = 1)
    {
        try
        {
            $rawdata = cfpr_host_compliance_list_all($username, $includes, $excludes, $rows, $page);
            
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
}

?>
