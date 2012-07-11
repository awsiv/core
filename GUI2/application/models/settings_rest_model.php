<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

class Settings_rest_model extends Cf_Model
{

    var $errors;
    var $restClient = null;

    function __construct()
    {
        parent::__construct();
        $this->load->library('mongo_db');
    }

    function setRestClient($restClient)
    {
        $this->restClient = $restClient;
    }

    /**
     * Gets app settings through rest interface
     *
     * @return array
     * @throws Exception
     */
    function get_app_settings()
    {
        $defaultFields = array(
            'appemail' => 'nias@asd.com',
            'rbac' => null,
            'authMode' => null,
            'ldapEncryption' => null,
            'ldapLoginAttribute' => null,
            'ldapHost' => '',
            "ldapBaseDN" => null,
            "ldapUsersDirectory" => null,
            "activeDirectoryDomain" => null,
            "blueHostHorizon" => 400
        );


        try
        {
            $settings = $this->restClient->get('/settings');
            $data = $this->checkData($settings);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                $dataMeregedWithDefault = array_merge($defaultFields, $data['data'][0]);
                return $dataMeregedWithDefault;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Pest_UnknownResponse $e)
        {
            throw new Exception('Cannot find the rest server.');
        }
        catch (Exception $e)
        {
            throw $e;
        }

    }

    function updateData($data)
    {
        if (is_array($data) && !empty($data))
        {
            try
            {
                           $data['authMode'] = 'database'; //TMP
                           $this->restClient->post('/settings', $data);
            }
            catch (Exception $e)
            {
                throw $e;
            }
        }
    }

}

?>
