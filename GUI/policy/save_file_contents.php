<?php 
		//$file=substr($_POST['file'], strrpos($filename, '.')
		$filetobesaved='../policies/'.$_POST['file'];
		$msg="";
		if(file_exists($filetobesaved)&&$_POST['filestats']=='old')
			{
				//open file for writng and place pointer at the end
				$handle = fopen($filetobesaved, 'w');
				if(!$handle)
				{
					$msg="couldn't open file <i>$filetobesaved</i>";
				}
			    $content=str_replace('\\', '' , $_POST['content']);
				fwrite($handle, $content);
				$msg= "success writing to file,".$content;
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
			    $content=str_replace('\\', '' , $_POST['content']);
				$content=str_replace('&gt;', '>' , $content);
				fwrite($handle, $content);
				$msg= "success writing to file";
				fclose($handle);
			}
			
			$details=array(
		    'title'=>$_POST['file'],
		    'path'=>'../policies/'.$_POST['file'],
		    'msg'=>$msg
	        );		
	       echo json_encode($details);
?>