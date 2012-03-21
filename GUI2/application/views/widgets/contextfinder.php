<?php
// generate item for include/exclude. This function too small, 
// and used only here, so don't move it to helper please

if (!function_exists('contextfinder_generate_item')) {
    function contextfinder_generate_item($name, $tabindex,  $value = '') {
        // if something changed here -chnage also in contextfinder.js new_el variable
        echo '<div class="item">
                <input tabindex="'.$tabindex.'" type="text" name="'.$name.'[]" value="'. $value .'"> 
                <a class="class_selector" href="'.site_url('widget/allclasses').'">&nbsp;</a>
                <a class="delete_condition" href="#">&nbsp;</a>
            </div>';
    }
}
?>
<?php

    //show X items if no include/exclude set
    $empty_items_on_page = 2; 
?>
<div class="contextfinder_wrapper">
  <form id="contextfinder"> 
     
        <table celpadding="0" cellspacing="0">
        <thead>    
            <tr>
                <td class="col1">Includes (.*+[])  <a class="add_condition" destination="includes" fieldname="include" href="#">Add condition <span>&nbsp;</span></a></td>
                <td class="col2">&nbsp;</td>   
                <td class="col1">Excludes (.*+[])  <a class="add_condition" destination="excludes" fieldname="exclude" href="#">Add condition <span>&nbsp;</span></a></td>
            </tr>
        </thead>
        <tbody>
        <tr>
            <td class="col1 includes">
<?php $tabindex = 1; ?>
                <?php        
                if ((isset($includes)) && count($includes)) {
                        foreach ($includes as $item => $value) {
                            echo contextfinder_generate_item('include', $tabindex, $value);
                            $tabindex++;
                        }
                    }
                    else {
                        for ($i=0; $i<$empty_items_on_page; $i++) {
                            echo contextfinder_generate_item('include', $tabindex, '');
                            $tabindex ++;
                        }
                    } 
                    ?>
            </td>
            <td class="col2"><a class="invert" href="#">&nbsp;</a></td>    
            <td class="col1 excludes">   
               
                <?php        
                if ((isset($excludes)) && count($excludes)) {
                        foreach ($excludes as $item => $value) {
                            echo contextfinder_generate_item('exclude', $tabindex, $value);
                            $tabindex ++;
                        }
                    }
                    else {
                        for ($i=0; $i<$empty_items_on_page; $i++) {
                            echo contextfinder_generate_item('exclude',$tabindex, '');
                            $tabindex ++;
                        }
                    } 
                    ?>
                
            </td>
        </tr>
        </tbody>
        </table>
 </form>   

<a class="green_btn" id="resetConditions" href="#"><span>Reset</span></a>    
<a class="green_btn" id="setConditions" href="#"><span>Set conditions</span></a>
</div>