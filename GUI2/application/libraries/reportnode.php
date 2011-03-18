<?php if (!defined('APPPATH')) exit('No direct script access allowed');

final class reportnode{
    public $id;
    public $name;
    public $data;
    public $children=array();

    function reportnode()
    {
        
    }
    
    function createnode($properties)
    {

       $temp =new reportnode();
       $temp->id=$properties['id'];
       $temp->name=$properties['name'];
       $temp->data=$properties['data'];
       if(count($properties['children'])>0)
       {
           foreach ($properties['children'] as  $child) {
               array_push($temp->children, $child);
           }
       }
       return $temp;
    }

    function getId()
        {
            return $this->id;
        }

     function setId($id)
     {
         $this->id=$id;
     }

     function setChildren($val)
     {
       $this->children=$val;
     }

}
?>
