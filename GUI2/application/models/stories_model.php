<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
class stories_model extends Cf_Model {
    
   function getStoryByName($name=Null){
   try {
            $rawdata = cfcon_get_story_by_name($name);
            $data = $this->checkData($rawdata);
            if ($data) {
                return $rawdata;
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
