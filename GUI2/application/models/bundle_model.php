<?php

class bundle_model extends Cf_Model
{

    /**
     *
     * @param type $username
     * @return type json, plain array
     */
    function getAllBundles($username)
    {
        try
        {
            $rawdata = cfpr_bundle_list_all($username);
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
     * get arguments if the bundle
     * @param string $username
     * @param string $type
     * @param string $bundle
     * @return $array
     */
    function getBundleArguments($username, $type, $bundle)
    {

        try
        {
            $rawdata = cfpr_bundle_arguments($username, $type, $bundle);
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
     * Classes used in the bundle
     * @param string $username
     * @param string $type
     * @param string $bundle
     * @return array
     */
    function getBundleClassesUsed($username, $type, $bundle)
    {

        try
        {
            $rawdata = cfpr_bundle_classes_used($username, $type, $bundle);
            $data = $this->checkData($rawdata);
            if ($this->hasErrors() == 0)
            {
                if (is_array($data))
                {
                    return $data;
                }
                else
                {
                    // if its a empty string we are getting, just return empty array
                    return array();
                }
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
     * Get bundle list using the passed bundle
     * @param string $username
     * @param string $bundle
     * @return array
     */
    function getBundleListByUsage($username, $bundle)
    {

        try
        {
            $rawdata = cfpr_bundle_list_by_bundle_usage($username, $bundle);

            $data = $this->checkData($rawdata);
            /* dont throw error if invalid data
             * return empty array
             */
            if (!$data)
            {
                $data = array();
            }
            return $data;
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

}

?>
