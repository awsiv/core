<?php

class License_model extends Cf_Model
{

    protected $rest = '';

    public function __construct()
    {
        parent::__construct();
    }

    /**
     *
     * @return Array of information
     */
    public function getLicenseDetails()
    {
        try
        {
            $apiInfo = $this->rest->get('/');
            $data = $this->checkData($apiInfo);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data['data'][0]['license'];
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
