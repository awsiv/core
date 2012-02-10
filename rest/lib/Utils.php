<?php

class Utils {
    
    public static function queryParam($name)
    {
        if (!array_key_exists($name, $_GET))
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
    
    public static function checkValidQueryParams($queryParams)
    {
        $defaultParams = array('sort', 'order', 'count', 'page');
        
        foreach (array_keys($_GET) as $value)
        {
            if (!in_array($value, $queryParams, true) && !in_array($value, $defaultParams, true))
            {
                throw new ResponseException($value . ' is not a valid query parameter',
                        Response::BADREQUEST);
            }
        }
    }

    public static function checkBoolean($var, $name)
    {
        if (!is_null($var) && ($var === "true" || $var === "false"))
        {
            throw new ResponseException($name . ' must be true/false',
                    Response::BADREQUEST);
        }
        return (bool)$var;
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
    
    public static function checkPromiseState($state)
    {
        if (!is_null($var) && !in_array($state, array('kept', 'notkept', 'repaired')))
        {
            throw new ResponseException('Valid promise states are kept, notkept and repaired',
                    Response::BADREQUEST);
        }
        return $state;
    }

    public static function InternalExceptionResponse($request, $exception)
    {
        $response = new Response($request);
        $response->body = $exception->getMessage();
        $response->code = Response::INTERNALSERVERERROR;
        return $response;
    }
}

