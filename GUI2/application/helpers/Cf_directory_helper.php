<?php
function handleWarning($errno, $errstr, $errfile, $errline) {
    // error was suppressed with the @-operator
    if (0 === error_reporting()) {
        return false;
    }

    throw new ErrorException($errstr, 0, $errno, $errfile, $errline);
}

function deleteAll($directory, $empty = false) 
{ 
    if(substr($directory,-1) == "/") { 
        $directory = substr($directory,0,-1); 
    } 

    if(!file_exists($directory) || !is_dir($directory)) { 
        return false; 
    } elseif(!is_readable($directory)) { 
        return false; 
    } else { 
        $directoryHandle = opendir($directory); 
        
        while ($contents = readdir($directoryHandle)) { 
            if($contents != '.' && $contents != '..') { 
                $path = $directory . "/" . $contents; 
                
                if(is_dir($path)) { 
                    deleteAll($path); 
                } else { 
                    unlink($path); 
                } 
            } 
        } 
        
        closedir($directoryHandle); 

        if($empty == false) { 
            if(!rmdir($directory)) { 
                return false; 
            } 
        } 
 
        return true; 
    }
}

 function  write_savefile($filetobesaved,$content)
   {
      set_error_handler("handleWarning");
                            $handle = fopen($filetobesaved, 'w');
				if(!$handle)
				{
					$msg="couldn't open file <i>$filetobesaved</i>";
				}
			    $content=str_replace('\\\\', '\\' , $content);

				$content=str_replace('&gt;', '>' , $content);
				if($_POST['agent']=='webkit')
				{
				$content=substr($content, 0, -3);
				}
				fwrite($handle, $content);
				fclose($handle);
                   return true;
   }
?>