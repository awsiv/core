<?php

/*
 This model includes the function that retruns the high level goals obtained from the policy files
 */

class goals_model extends Cf_Model
{
    /**
     *
     * @return type
     */
    function getAllGoals()
    {
        try
        {
            $rawdata = cfpr_list_business_goals();
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
