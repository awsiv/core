<div id="infoMessage"></div>
<div class="form">
<?php
echo form_open('eventTracker/create');
?>
<p>
   <label for="reportType">Report Type<span class="required"></span></label> 
   <select id="reportType" name="reportType">
       <option>Promise not Kept</option>
       <option>Promise Repaired</option> 
   </select>
</p>
<p>
    <label for="resource">Unique identifier<span class="required"></span></label> 
    <input id="resource" type="text" name="resource" value=""  /> 
</p>
<p>
   <label for="time">Start Time<span class="required"></span></label> 
    <input id="time" type="text" name="time" value=""  />
</p>
<p>
    <label for="trackerName">Name<span class="required"></span></label> 
    <input id="trackerName" type="text" name="trackerName" value=""  />
</p>
<p id="btnholder">
    <label></label>
     <span class="green_btn">
           <input class="green_btn" type="submit" value="Create">
     </span>
</p>
<?
echo form_close();
?>
</div>

