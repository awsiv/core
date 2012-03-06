<?php
if (!defined('BASEPATH'))
    exit('No direct script access allowed');

function roles_checkbox($value, $id, $class) {

    $value = str_replace(' ', '', $value);
    $arr = explode(',', $value);

    $i=1;
    $first = true;
    $fist_item_class = ' class="first"';
 
    

$ret = '<ul id="'.$id .'" class="'.$class.'">';

    foreach ($arr as $item) {
            $ret.= '<li id="" itemid="'.$i.'" '.($first === true ?  $fist_item_class : "").' >';
            $data = array(
                'name'        => "crxi[]",
                'id'          => '',
                'value'       => $item,
                'checked'     => TRUE,
                'style'       => '',
                'class'       => "selectit"
                );
            $ret .= form_checkbox($data);  
            $ret .=  form_label($item, $item);
        $ret .=  "</li>";  
        $i++;
        $first = false;
    }

$ret .= '</ul>';

return $ret;
}
?>