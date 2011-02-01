<?php
        $file_path=$_POST['file_path'];
		
		$filename= basename($file_path);
		$contents = file_get_contents (realpath($file_path));
		//$information=get_file_info($file_path);
		$details=array(
		'filename'=>$filename,
		'content'=>$contents
	     );
		
	    echo json_encode($details);
?>