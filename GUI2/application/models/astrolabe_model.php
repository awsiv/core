<?php

// for compatibility with PHP 5.1
function profile_id($profile)
{
    return $profile['profileId'];
}

class Astrolabe_Model extends CI_Model {

    function __construct() {
        parent::__construct();
        $this->load->library('mongo_db');
    }

    function profile_list($username) {
        if (is_null($username)) {
            return NULL;
        }

        $data = $this->mongo_db->select(array('profileId'))->
                                 where(array('username' => $username))->
                                 get('astrolabe.profile');

        return array_map("profile_id", $data);
    }

    function profile_get($username, $profileId) {
        if (!is_null($username) && !is_null($profileId)) {
            $data = $this->mongo_db->where(
                    array('username' => $username,
                          'profileId' => $profileId))->
                    limit(1)->
                    get_object('astrolabe.profile');
            if (is_object($data)){
                return $data;
            }
        }
        return null;
    }

    function profile_insert($username, $profileId, $nodeDescriptionList) {
        if (is_null($username) || is_null($profileId) && is_null($nodeDescriptionList)) {
            return false;
        }

        $id = $this->mongo_db->insert('astrolabe.profile', array(
            'username' => $username,
            'profileId' => $profileId,
            'data' => $nodeDescriptionList));

        return $id !== NULL;
    }

    function profile_delete($username, $profileId) {
        if (is_null($username) || is_null($profileId)) {
            return false;
        }

        $this->mongo_db->where(array(
            'username' => $username,
            'profileId' => $profileId
        ))->delete('astrolabe.profile');

        return true;
    }
}

?>
