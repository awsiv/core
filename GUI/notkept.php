<?php
cfpr_header("promises not kept","normal");

?>

<div id="selector">

<?php
$ret = cfpr_report_patch_avail($hostkey,$name,$version,$arch,$regex,NULL);
?>


</div>
     
<?php
cfpr_footer();
?>