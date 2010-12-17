<?php 
       session_start();
		//$file=substr($_POST['file'], strrpos($filename, '.')
		$working_dir = realpath('../policies');
		if(!file_exists($working_dir.'/'.session_id()))
		  {
			mkdir($working_dir.'/'.session_id(),0700);
		  }
		$filetobesaved='../policies/'.session_id().'/'.$_POST['file'];
		$msg="";
		$written=false;
		if(file_exists($filetobesaved)&&$_POST['filestats']=='old')
			{
				//open file for writng and place pointer at the end
				$handle = fopen($filetobesaved, 'w');
				if(!$handle)
				{
					$msg="couldn't open file <i>$filetobesaved</i>";
				}
			    $content=str_replace('\\\\', '\\' , $_POST['content']);
//			    $content=str_replace('\\"', '"' , $content);
                $content=str_replace('&gt;', '>' , $content);
				if($_POST['agent']=='webkit')
				{
				$content=substr($content, 0, -3);
				}
				fwrite($handle, $content);
				$msg= "success writing to file,".$content;
				$written=true;
				fclose($handle);
			}
			elseif(file_exists($filetobesaved)&&$_POST['filestats']=='new')
			{
				$msg= "couldn't write file <i>$filetobesaved</i>,File with same name exist in repository";
			}
			else
			{
				//$msg= "file <i>$filetobesaved</i> doesn't exists";
				$handle = fopen($filetobesaved, 'w');
				if(!$handle)
				{
					$msg="couldn't open file <i>$filetobesaved</i>";
				}
			    $content=str_replace('\\\\', '\\' , $_POST['content']);
//			    $content=str_replace('\\"', '"' , $content);
				$content=str_replace('&gt;', '>' , $content);
				if($_POST['agent']=='webkit')
				{
				$content=substr($content, 0, -3);
				}
				fwrite($handle, $content);
				$written=true;
				$msg= "success writing to file";
				fclose($handle);
			}
			
			$details=array(
			'status'=>$written,
		    'title'=>$_POST['file'],
		    'path'=>'../policies/'.session_id().'/'.$_POST['file'],
		    'msg'=>$msg
	        );		
	       echo json_encode($details);
?>