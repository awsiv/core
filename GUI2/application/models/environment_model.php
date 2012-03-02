<?php

class environment_model extends Cf_Model
{

    function getEnvironmentList($username)
    {
        try
        {
            //cfpr_environment_list returns array 
            $data = cfpr_environment_list($username);
            if (is_array($data))
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
            log_message('error', $e->getMessage() . " " . $e->getFile() . " " . $e->getLine());
            throw $e;
        }
    }

}

?>
