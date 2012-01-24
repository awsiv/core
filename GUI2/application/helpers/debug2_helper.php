<?php

if (!defined('BASEPATH'))
    exit('No direct script access allowed');

if (!function_exists('debug2')) {

    function debug2($data, $descr = false) {
        $dub = debug_backtrace();
        
        if ($descr !== false)
        {
            echo "<br /> ********** ". $descr ."***************<br />";
        }
        echo '<pre><span >';
        echo $dub[0]['file'] . "<br/>On Line Number ";
        echo $dub[0]['line'] . "<br/></span>";
        print_r($data);

        echo '</pre>';
    }

}
?>
