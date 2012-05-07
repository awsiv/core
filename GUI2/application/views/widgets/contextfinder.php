<?php
// generate item for include/exclude. This function too small, 
// and used only here, so don't move it to helper please

if (!function_exists('contextfinder_generate_item')) {
    function contextfinder_generate_item($name, $tabindex, $href = 'widget/allclasses', $value = '') {
        // if something changed here -chnage also in contextfinder.js new_el variable
        echo '<div class="item">
                <input tabindex="'.$tabindex.'" type="text" name="'.$name.'[]" value="'. $value .'"> 
                <a class="class_selector" href="'.site_url($href).'">&nbsp;</a>
                <a class="delete_condition" href="#">&nbsp;</a>
            </div>';
    }
}
?>
<?php

    //show X items if no include/exclude set
    $empty_items_on_page = 2; 
    
 //
    
?>

<div <?php (isset($html_id) ? 'id="'.$html_id.'"' : '') ?> class="contextfinder_wrapper <?php echo ( (isset($embedded) && $embedded==true) ? 'embedded' : '') ?>">
    
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
                $name = 'include';  
                
                if (isset($fields['left']['name']) && $fields['left']['name'] != '') {
                    $name = $fields['left']['name'];   
                }
                
                $href= 'widget/allclasses';  //default
                
                if(isset($fields['left']['href'])) {
                    $href = $fields['left']['href'];
                }
                
                if (isset($fields['left']['values']) && $fields['left']['values'] != '') {
                $includes = $fields['left']['values'];
                    foreach ($includes as $item => $value) {
                        echo contextfinder_generate_item($name, $tabindex, $href, $value );
                        $tabindex++;
                    }
                } else {
                    for ($i = 0; $i < $empty_items_on_page; $i++) {
                        echo contextfinder_generate_item($name, $tabindex, $href,  '');
                        $tabindex++;
                    }
                }
                    ?>
            </td>
            <td class="col2"><a class="invert" href="#">&nbsp;</a></td>    
            <td class="col1 excludes">   
               
                <?php        
                $name = 'exclude';  

                if (isset($fields['right']['name']) && $fields['right']['name'] != '') {
                    $name = $fields['right']['name'];   
                }
                
                $href= 'widget/allclasses'; //default
                
                if(isset($fields['right']['href'])) {
                    $href = $fields['right']['href'];
                }                
                
                if (isset($fields['right']['values']) && $fields['right']['values'] != '') {
                $excludes = $fields['right']['values'];
                    foreach ($excludes as $item => $value) {
                        echo contextfinder_generate_item($name, $tabindex, $href, $value);
                        $tabindex++;
                    }
                } else {
                    for ($i = 0; $i < $empty_items_on_page; $i++) {
                        echo contextfinder_generate_item($name, $tabindex, $href, '');
                        $tabindex++;
                    }
                }
            ?>
            </td>
        </tr>
        </tbody>
        </table>
 </form>   
<?php if ( isset($doNotShowButtons) && $doNotShowButtons != true) { ?>
        <a class="green_btn" id="resetConditions" href="#"><span>Reset</span></a>    
        <a class="green_btn" id="setConditions" href="#"><span>Set conditions</span></a>
<?php } ?>    
    
</div> 