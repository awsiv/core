<?php
require_once SPARKPATH.'restclient/2.1.0/libraries/Rest.php';
class cf_rest extends Rest {
    
    function setCredentials($user,$pass) {
        $this->http_user = $user;
        $this->http_pass = $pass;
    }
     
    function setAuthMode($mode){
        $this->http_auth=$mode;
    }
    
}
?>
