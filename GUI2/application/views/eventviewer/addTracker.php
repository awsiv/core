<div class="form">
<?php
echo form_open('eventTracker/create');
?>

<p>
   <label for="reportType">Report Type<span class="required"></span></label> 
   <select id="reportType" name="reportType">
       <option value="not_kept">Promise not Kept</option>
       <option value="repaired">Promise Repaired</option> 
   </select>
</p>
<p>
    <label for="resource">Watch<span class="required"></span></label> 
    <input id="resource" type="text" name="resource" value=""  /> 
</p>
<p>
   <label for="time">Start Time<span class="required"></span></label> 
    <input id="time" type="text" name="time" value=""  />
</p>
   
   <!-- <p>
        <a href="#" id="startViewhandle">Redefine tracker</a>
    </p>-->
    <p>
        <label for="trackerName">Save <span class="required"></span></label> 
        <input id="trackerName" type="text" name="trackerName" value=""  />
    </p>
    
    <p id="btnholder">
        <label></label>
         <a class="green_btn" id="testBtn">
           <span class="green_btn">Start</span>
         </a> 
        
         <span class="green_btn" id="submitForm">
               <input class="green_btn" type="submit" value="Save" />
         </span> 
         
    </p>
   
<?
echo form_close();
?>
</div>
<script type="text/javascript">
$( "input:text[name=time]" ).datetimepicker({
      dateFormat: 'yy-mm-dd',
      timeFormat: 'hh:mm:ss'
});
</script>
