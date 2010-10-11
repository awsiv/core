<?php
   	    $status="unchanged";
		$root="../";
  		$file_path=$root.$_POST['file_path'];
		$contents = file_get_contents($file_path);
		$newcontents=str_replace('\u200b', '' , $_POST['newcontents']);
		if($_POST['agent']=='webkit')
		{
		$newcontents=substr($newcontents, 0, -3);
		}
		if(strcmp($contents,$newcontents))
		{
		$status="changed";	
		}
		$data=array(
		'status'=>$status,
		//'old'=>$contents,
		//'newd'=>$newcontents,
	     );
		echo json_encode($data);
?>