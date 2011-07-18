<?php
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
class Settings_model extends CI_Model {

    var $collectionName = 'appsettings';
    var $errors;

      function __construct() {
        parent::__construct();
        $this->load->library('mongo_db');
    }

    function insert_app_settings($data) {
        if (!is_array($data)) {
            $this->setError('Invalid parameter passed to function');
            return false;
        }
            $id = $this->mongo_db->insert($this->collectionName, $data);
            if ($id) {
             //  $obj= $this->mongo_db->where(array('_id' =>new MongoId($id)))->get_object($this->collectionName);
              return true;
           }
        return false;
    }

   function get_app_settings(){
       $data=$this->mongo_db->limit(1)->get_object($this->collectionName);
      if (is_object($data)){
          return $data;
          }
          return false;
   }

   function update_app_settings($data,$id=null){
    if(!is_null($id))
         {
             $this->mongo_db->where(array('_id' =>new MongoId($id)))->update($this->collectionName,$data);
             return true;
         }
      return false;
   }

   function app_settings_get_item($item)
   {
      $data=$this->mongo_db->select(array($item))->limit(1)->get_object($this->collectionName);
      if (is_object($data)){
          return $data->$item;
          }
      return false;
   }

   

}

?>
