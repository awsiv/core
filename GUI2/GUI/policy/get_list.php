<?php
    session_start();
	$root='../';
	$checked = false;
	if($_GET['op']=="ischeckedout")
	{
		if(file_exists($root . 'policies/'.session_id().'/') ) {
			$files = scandir($root . 'policies/'.session_id().'/');
			natcasesort($files);
		    if( count($files) > 2 ) {
			 $checked = true;	
			}
		}
		echo $checked;
		return;
	}
	
echo "<ul id=\"policies_list_new\" class=\"jqueryFileTree\">";
	$_POST['dir'] = urldecode($_POST['dir']);
	    if(file_exists($root . $_POST['dir'].session_id().'/') ) {
	    $files = scandir($root . $_POST['dir'].session_id().'/');
	    natcasesort($files);
		if( count($files) > 2 ) { /* The 2 accounts for . and .. */
			//previus echo
			// All dirs
			/*foreach( $files as $file ) {
				if( file_exists($root . $_POST['dir'] . $file) && $file != '.' && $file != '..' && is_dir($root . $_POST['dir'] . $file) ) {
					echo "<li class=\"directory collapsed\"><a href=\"#\" rel=\"".$root . htmlentities($_POST['dir'] . $file) . "/\">" . htmlentities($file) . "</a></li>";
				}
			}*/
			// All files
			$i=1;
			foreach( $files as $file ) {
				if( file_exists($root . $_POST['dir'].session_id().'/' . $file) && $file != '.' && $file != '..' && !is_dir($root . $_POST['dir'] . session_id().'/'.$file) ) {
					$ext = preg_replace('/^.*\./', '', $file);
					echo "<li class=\"file ext_$ext\"><a href=\"#\" rel=\"".$root . htmlentities($_POST['dir'].session_id().'/' . $file) . "\" id=\"policy_".$i."\">" . htmlentities($file) .  "</a></li>";
					$i++;
				}
			}
		//previus echo ul		
		}
      }
	 echo "</ul>";
?>