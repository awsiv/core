<?php

class Utils
{
    const ERRID_UNKNOWN = 0;
    const ERRID_SUCCESS = 1;
    const ERRID_DBCONNECT = 2;
    const ERRID_DBCLOSE = 3;
    const ERRID_DB_OPERATION = 4;
    const ERRID_ARGUMENT_MISSING = 5;
    const ERRID_ARGUMENT_WRONG = 6;
    const ERRID_BUFFER_FULL = 7;
    const ERRID_ITEM_NONEXISTING = 8;
    const ERRID_ITEM_EXISTS = 9;
    const ERRID_ITEM_MULTIPLE = 10;
    const ERRID_RBAC_DISABLED = 11;
    const ERRID_RBAC_ACCESS_DENIED = 12;
    const ERRID_DATA_UNAVAILABLE = 13;
    const ERRID_HOST_NOT_FOUND = 14;
    const ERRID_ACCESS_DENIED = 15;
    const ERRID_ACCESS_DENIED_EXTERNAL = 16;

    public static function baseUri()
    {
        if ($_SERVER['HTTPS'])
        {
            $prot = 'https://';
        }
        else
        {
            $prot = 'http://';
        }

        return $prot . $_SERVER['SERVER_ADDR'] . '/api/';
    }

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

    public static function checkBoolean($var, $name)
    {
        if (!is_null($var))
        {
            if (strtolower($var) === "true")
            {
                return true;
            }
            else if (strtolower($var) === "false")
            {
                return false;
            }
            else
            {
                throw new ResponseException("query parameter '" . $name . "' must be an true or false",
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

    public static function prefixedHostKey($hostkey)
    {
        if ($hostkey && substr($hostkey, 0, 4) !== 'SHA=')
        {
            return 'SHA=' . $hostkey;
        }
        return $hostkey;
    }

    public static function CFAPIErrorCodeToHttpCode($cfapi_code)
    {
        switch ($cfapi_code)
        {
            case Utils::ERRID_SUCCESS:
                return 200;
            case Utils::ERRID_ITEM_NONEXISTING:
                return 404;
            case Utils::ERRID_ARGUMENT_MISSING:
            case Utils::ERRID_ARGUMENT_WRONG:
                return 400;
            case Utils::ERRID_ITEM_EXISTS:
                return 409;
            case Utils::ERRID_RBAC_ACCESS_DENIED:
            case Utils::ERRID_ACCESS_DENIED:
            case Utils::ERRID_ACCESS_DENIED_EXTERNAL:
                return 403;
            case Utils::ERRID_DATA_UNAVAILABLE:
            case Utils::ERRID_HOST_NOT_FOUND:
                return 404;

            default:
                return 500;
        }
    }
}

