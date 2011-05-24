<?php

function is_svn_checked_out($path,$username) {
    $checked = false;
    if (file_exists($path . $username . '/')) {
        $files = scandir($path . $username . '/');
        natcasesort($files);
        if (count($files) > 2) {
            $checked = true;
        }
    }
    return $checked;
    
}

?>
