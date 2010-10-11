<?php

	//$data['policies']=$this->db_policy->get_policies();
	//$this->load->view('policy_list',$data);
	$root='../';
	$_POST['dir'] = urldecode($_POST['dir']);
	    if(file_exists($root . $_POST['dir']) ) {
	    $files = scandir($root . $_POST['dir']);
	    natcasesort($files);
		if( count($files) > 2 ) { /* The 2 accounts for . and .. */
			echo "<ul id=\"policies_list_new\" class=\"jqueryFileTree\">";
			// All dirs
			foreach( $files as $file ) {
				if( file_exists($root . $_POST['dir'] . $file) && $file != '.' && $file != '..' && is_dir($root . $_POST['dir'] . $file) ) {
					echo "<li class=\"directory collapsed\"><a href=\"#\" rel=\"".$root . htmlentities($_POST['dir'] . $file) . "/\">" . htmlentities($file) . "</a></li>";
				}
			}
			// All files
			$i=1;
			foreach( $files as $file ) {
				if( file_exists($root . $_POST['dir'] . $file) && $file != '.' && $file != '..' && !is_dir($root . $_POST['dir'] . $file) ) {
					$ext = preg_replace('/^.*\./', '', $file);
					echo "<li class=\"file ext_$ext\"><a href=\"#\" rel=\"".$root . htmlentities($_POST['dir'] . $file) . "\" id=\"policy_".$i."\">" . htmlentities($file) .  "</a></li>";
					$i++;
				}
			}
			echo "</ul>";	
		}
      }
	 
?>