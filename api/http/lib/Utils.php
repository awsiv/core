<?php

class Utils {

    public static function queryParam($name, $defaultValue = NULL)
    {
        if (!array_key_exists($name, $_GET))
        {
            return $defaultValue;
        }
        else
        {
            return $_GET[$name];
        }
    }

    public static function checkValidQueryParams($queryParams)
    {
        $defaultParams = array('from', 'to', 'count', 'page');

        foreach (array_keys($_GET) as $value)
        {
            if (!in_array($value, $queryParams, true) && !in_array($value, $defaultParams, true))
            {
                throw new ResponseException($value . ' is not a valid query parameter',
                        Response::BADREQUEST);
            }
        }
    }

    public static function checkInteger($var, $name)
    {
        if (!is_null($var))
        {
            if (ctype_digit($var))
            {
                return (int)$var;
            }
            else
            {
                throw new ResponseException("query parameter '" . $name . "' must be an integer",
                        Response::BADREQUEST);
            }
        }
        else
        {
            return NULL;
        }
    }

    public static function checkPromiseState($state)
    {
        if (!is_null($state) && !in_array($state, array('kept', 'notkept', 'repaired')))
        {
            throw new ResponseException('Valid promise states are kept, notkept and repaired',
                    Response::BADREQUEST);
        }
        return $state;
    }
}

