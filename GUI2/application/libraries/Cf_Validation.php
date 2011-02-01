<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of Cf_Validation
 *
 * @author sudhir
 */
class Cf_Validation extends CI_Validation{

    function __construct()
    {
        parent::CI_Validation();
    }
    
    function set_checkbox($field = '', $value = '')
	{
            if ($field == '' OR $value == '')
                    {
                        return '';
                    }        
                    if(isset($_POST[$field])) {
                        if ($_POST[$field] == $value || ( is_array($_POST[$field]) && in_array($value, $_POST[$field])) )
                        {
                            return ' checked="checked"';
                        }
                    }
                elseif(isset($this->$field)) {
                    if($this->$field==$value || ( is_array($this->$field) && array_search($value, $this->$field)!==FALSE) ) {
                        return ' checked="checked"';
                    }
                }
	}
        
}
?>
