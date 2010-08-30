<fieldset>
<legend>Bundle characterstics</legend>

	
	<!-- <select><option>Tomact</option><option>apache</option></select>-->
			<!--  <div class="combo">
			<label for="state">Application: </label>
			<input type="text" name="Application" id="Application" value="" size="20">
			<ul>
			<li>Tomcat</li>
			<li>Apache</li>
			</ul>
			</div>-->
		<div class="grid_10 alpha">
			<div class="grid_6 alpha">
				<label >Application name</label><input type="text" id="application"></input>
				<select id="applist" class="grid_2">
				 <option>Tomcat</option>
				 <option>Apache</option>
				</select>
			</div>
			<div class="grid_2 omega">
				<label>Version</label><input type="text" id="version" value="3.0" class="spin"/>
			</div>
				
           <a href="#" id="createbdl" class="grid_2">Create Bundle</a>
           <div class="grid_6 alpha">
				<label >Operating System</label><input type="text" id="os"></input>
				<select id="oslist" class="grid_2">
				 <option>Windows</option>
				 <option>RHCE</option>
				</select>
			</div>
			<div class="grid_2 omega">
				<label>Version</label><input type="text" id="osversion" value="4.0" class="spin"/>
			</div>
		</div>
		 <div class="grid_4 omega">
	           <fieldset>
	           <legend>Help/Information</legend>
	           <p style='font-size:11px'>Type in the name of the application that this bundle is
	           about.If System settings Type in the Operating System as Application name also</p>
	           </fieldset>
           </div>

</fieldset>
<script type="text/javascript">
$(".spin").spinbox({
	  min: 1.0,	   // Set lower limit or null for no limit.
	  max: 7.0,  // Set upper limit or null for no limit.
	  step: 0.1	 // Set increment size.
	});

$('#createbdl').click(function(){
    alert("bundle created");	
});

$('#application').combobox($('#applist'));
$('#os').combobox($('#oslist'));
//new combo('Application','#999','#fff');
</script>

