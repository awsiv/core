<?php

class virtual_bundle_model extends Cf_Model {

    /**
     * function to return virtual bundle report  data
     * @return array
     */
    function getVirtualBundleData($virtualBundle=null, $username=null, $recordPerPage=null, $currentPage=1) {


        try {
            $rawdata = cfcon_local_report_virtualbundle($username, $virtualBundle, $recordPerPage, $currentPage);
            $data = $this->checkData($rawdata);
            if ($data) {
                return $data;
            } else {
                return false;
            }
        } catch (Exception $e) {
            log_message('error', $e->getMessage());
            throw $e;
        }
    }

}

?>