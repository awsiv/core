<?php

class vitals_model extends Cf_Model {

    /**
     * Gets the sorted list of vitals for a given host
     * @param type $username
     * @param type $hostkey
     * @return type array
     */
    function getVitalsList($username, $hostkey) {
        $json = cfpr_vitals_list($username, $hostkey);
        $returnArray = json_decode($json, true);
        return $returnArray;
    }

    /**
     * Returns data for week view in vitals
     * @param type $username
     * @param type $hostkey
     * @param type $observables
     * @return string JSON  
     */
    function getVitalsWeekViewJson($username, $hostkey, $observables) {
        try {
            return cfpr_vitals_view_week($username, $hostkey, $observables);
        } catch (Exception $e) {
            $error = generate_errormessage($e);
            return $error;
        }
    }
    
    /**
     * Get weeks view analysis
     * @param type $username
     * @param type $hostkey
     * @param type $observables
     * @return array 
     */
     function getVitalsWeekAnalysis($username, $hostkey, $observables) {
        try {
            return json_decode(cfpr_vitals_analyse_week($username, $hostkey, $observables), true);
        } catch (Exception $e) {
            $error = generate_errormessage($e);
            return $error;
        }
    }

    /**
     *
     * @param type $username
     * @param type $hostkey
     * @return timestamp 
     */
    function getVitalsLastUpdate($username, $hostkey) {
        try {
            $hostData = cfpr_getlastupdate($username, $hostkey);
            return $hostData;
        } catch (Exception $e) {
            $error = generate_errormessage($e);
            return $error;
        }
    }
    
    
    /**
     * Returns data for magnified view in vitals
     * @param type $username
     * @param type $hostkey
     * @param type $observables
     * @return string JSON 
     */
    function getVitalsMagnifiedViewJson($username, $hostkey, $observables) {
        try {
            
            return cfpr_vitals_view_magnified($username, $hostkey, $observables);
            
        } catch (Exception $e) {
            $error = generate_errormessage($e);
            return $error;
        }
    }
    
    /**
     * Get magnified view analysis
     * @param type $username
     * @param type $hostkey
     * @param type $observables
     * @return array 
     */
     function getVitalsMagnifiedAnalysis($username, $hostkey, $observables) {
        try {
            return json_decode(cfpr_vitals_analyse_magnified($username, $hostkey, $observables), true);
        } catch (Exception $e) {
            $error = generate_errormessage($e);
            return $error;
        }
    }
    
    
    /**
     * Returns data for year view in vitals
     * @param type $username
     * @param type $hostkey
     * @param type $observables
     * @return string JSON 
     */
    function getVitalsYearViewJson($username, $hostkey, $observables) {
        try {
            
            return cfpr_vitals_view_year($username, $hostkey, $observables);
            
        } catch (Exception $e) {
            $error = generate_errormessage($e);
            return $error;
        }
    }
    
    /**
     * Get year view analysis
     * @param type $username
     * @param type $hostkey
     * @param type $observables
     * @return array 
     */
     function getVitalsYearAnalysis($username, $hostkey, $observables) {
        try {
            return json_decode(cfpr_vitals_analyse_year($username, $hostkey, $observables), true);
        } catch (Exception $e) {
            $error = generate_errormessage($e);
            return $error;
        }
    }
    
    /**
     * Returns data for histogram view in vitals
     * @param type $username
     * @param type $hostkey
     * @param type $observables
     * @return string JSON 
     */
    function getVitalsHistogramViewJson($username, $hostkey, $observables) {
        try {
            
            return cfpr_vitals_view_histogram($username, $hostkey, $observables);
            
        } catch (Exception $e) {
            $error = generate_errormessage($e);
            return $error;
        }
    }
    
    /**
     * Get year histogram analysis
     * @param type $username
     * @param type $hostkey
     * @param type $observables
     * @return array 
     */
     function getVitalsHistogramAnalysis($username, $hostkey, $observables) {
        try {
            return json_decode(cfpr_vitals_analyse_histogram($username, $hostkey, $observables), true);
        } catch (Exception $e) {
            $error = generate_errormessage($e);
            return $error;
        }
    }   

}

?>
