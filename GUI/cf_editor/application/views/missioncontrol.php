<div id="mc_options" class="grid_15">
	<a href='#' class="grid_3 alpha">Create Bundle</a>
	<a href='#' class="grid_3">Pending Bundles</a>
	<a href='#'class="grid_3 omega">Approved Bundles</a>
</div>
<div class="grid_15">
 	<span id='nav' class="grid_14">Home</span>
</div>
<div id="mc_work_area" class="grid_15 omega">

</div>

<script type="text/javascript">
$('a','#mc_options').click(function(){
	var path="";
	$('#nav').html('Home > '+$(this).html());
	if($(this).html()=='Create Bundle')
	{
		path="<?php echo site_url('policy/loadbundleview')?>/";
		$('#mc_work_area').load(path);
	}
});
</script>
