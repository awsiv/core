<?php
        session_start();
   	    $status="unchanged";
		$root="../";
  		$file_path=$root."policies/".session_id().'/'.$_POST['file'];
		$contents = file_get_contents($file_path);

                $newcontents=str_replace('\\\\', '\\' , $_POST['newcontents']);
//                $newcontents=str_replace('\\"', '"' , $newcontents);
                $newcontent=str_replace('&gt;', '>' , $newcontents);

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
		'old'=>$contents,
		'newd'=>$newcontents,
	     );
		echo json_encode($data);
?>