<?php

class Utils {
    
    public static function queryParam($name)
    {
        if (is_null($_GET[$name]))
        {
            return NULL;
        }
        else if (strcasecmp($_GET[$name], 'true') == 0)
        {
            return TRUE;
        }
        else if (strcasecmp($_GET[$name], 'false') == 0)
        {
            return FALSE;
        }
        else
        {
            return $_GET[$name];
        }
    }

    public static function checkInteger($var, $name)
    {
        if (!is_null($var) && !ctype_digit($var))
        {
            throw new ResponseException($name . ' must be an integer',
                    Response::BADREQUEST);
        }
        return (int)$var;
    }
}

