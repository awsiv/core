<?php
if (!defined('BASEPATH'))
    exit('No direct script access allowed');
/**
 * Create html list from string.  Used for showing classes and bundles at create/edit roles page
 * 
 * @param string $value  string which can be converted to array with explode
 * @param string $name - field name for checkboxes
 * @param string $id  ul id=$id
 * @param type $class ul class=$class
 * @return string 
 */
function create_html_list_from_string($value, $name, $id, $class) {

    //convert string to array
    $value = str_replace(' ', '', $value);
    $arr = explode(',', $value);

    $i=1;
    $first = true;
    $fist_item_class = ' class="first"';
 

$ret = '<ul id="'.$id .'" class="'.$class.'">';

    foreach ($arr as $item) {
            $ret.= '<li itemid="'.$i.'" '.($first === true ?  $fist_item_class : "").' >';
            $data = array(
                'name'        => $name,
                'id'          => $item,
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