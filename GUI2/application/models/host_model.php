<?php

class host_model extends Cf_Model
{

    /**
     * Get the list of host by specified colors
     * @param string color (green,red,yellow,blue,black)
     * @param string $usertname
     * @param string $includes
     * @param string $excludes
     * @param string $rows
     * @param string $page
     * @return array
     * @throws Exception
     */
    function getHostByColor($color, $username, $includes = array('.*'), $excludes = array(), $rows = 10, $page = 1)
    {
        try
        {
            $rawdata = cfpr_host_compliance_list($username, $color, $includes, $excludes, $rows, $page);
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
            generate_errormessage($e);
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $ipregx
     * @param type $rows
     * @param type $page
     * @return type array data
     */
    function getHostByIP($username, $ipregx, $rows = 15, $page = 1)
    {
        try
        {
            $rawdata = cfpr_host_list_by_ip_rx($username, $ipregx, $rows, $page);
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
            log_message('error', $e->getMessage() . " File:" . $e->getFile() . " line:" > $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostregx
     * @param type $rows
     * @param type $page
     * @return type array
     */
    function getHostByName($username, $hostregx, $rows = 15, $page = 1)
    {
        try
        {
            $rawdata = cfpr_host_list_by_name_rx($username, $hostregx, $rows, $page);
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
            log_message('error', $e->getMessage() . " File:" . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostKey
     * @return type string
     */
    function getHostColor($username, $hostKey)
    {
        try
        {
            $rawdata = cfpr_host_compliance_colour($username, $hostKey);
            if (is_string($rawdata))
            {
                return $rawdata;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " File:" . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @return type string hostname for a given key
     */
    function getHostName($username, $hostkey)
    {
        try
        {
            $rawdata = cfpr_host_by_hostkey($username, $hostkey);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data[0];
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " File:" . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @return type String Ip address for given hostkey
     */
    function getHostIp($username, $hostkey)
    {
        try
        {
            $rawdata = cfpr_host_by_hostkey($username, $hostkey);
            $data = $this->checkData($rawdata);
            if (is_array($data))
            {
                return $data[1];
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " File:" . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @return type timestamp
     * Need to check empty form the controller side
     */
    function getLastUpdate($username, $hostkey)
    {
        try
        {
            $rawdata = cfpr_getlastupdate($username, $hostkey);
            //$data = $this->checkData($rawdata);
            return $rawdata;
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " File:" . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @param type $scope
     * @param type $lval
     * @return type depends on the variable type
     */
    function getHostVariable($username, $hostkey, $scope, $lval)
    {
        try
        {
            $rawdata = cfpr_report_vars($username, $hostkey, $scope, $lval, NULL, NULL, false, array(), array(), "var-name", true, 1, 1);
            $data = $this->checkData($rawdata);
            if (is_array($data) && key_exists($scope, $data))
            {
                return $data[$scope]['data'][0][3];
            }
            else
            {
                return false;
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " File:" . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @return type string
     */
    function getNetworkSpeed($username, $hostkey)
    {
        try
        {
            $rawdata = cfpr_network_speed($username, $hostkey);
            $data = $this->checkData($rawdata);
            if ($data)
            {
                return $data;
            }
            else
            {
                return false;
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " File:" . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $username
     * @param type $rows
     * @param type $page
     * @return type
     */
    function getComplianceList($username, $includes = array('.*'), $excludes = array(), $rows = 15, $page = 1)
    {
        try
        {

            $rawdata = cfpr_host_compliance_list_all($username, $includes, $excludes, $rows, $page);
            $data = $this->checkData($rawdata);
            if ($data)
            {
                return $data;
            }
            else
            {
                return false;
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " File: " . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
    }

    /**
     *
     * @param type $key
     * deletes the host for supplied hostkey
     */
    function deleteHost($username, $key)
    {
        try
        {
            $data = cfpr_delete_host($username, $key);
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " File: " . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
    }

    /**
     * Gets total number of host count of specified color
     * @param type $username
     * @param type $color
     * @return type
     * @throws Exception
     */
    function getHostCount($username, $color = NULL, $includes = array(), $excludes = array())
    {
        try
        {
            $data = cfpr_host_count($username, $color, $includes, $excludes);
            if (is_numeric($data))
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
            log_message('error', $e->getMessage() . " File: " . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
    }
    
    
    function getHostMeter($username,$hostkey){
         try
        {
            $rawdata = cfpr_host_meter($username, $hostkey);
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
            log_message('error', $e->getMessage() . " File: " . $e->getFile() . ' line:' . $e->getLine());
            throw $e;
        }
        
    }   
    
}

?>
