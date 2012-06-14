<?php

require_once 'Utils.php';

class DefaultParameters
{
    public static function check()
    {
        DefaultParameters::from();
        DefaultParameters::to();
        DefaultParameters::page();
        DefaultParameters::count();
    }

    public static function format()
    {
        return 'json-v1';
    }

    public static function from()
    {
        $from = Utils::checkInteger(Utils::queryParam('from', "0"), 'from');
        if ($from < 0)
        {
            throw new ResponseException("query parameter 'from' must be non-negative",
                        Response::BADREQUEST);
        }
        return $from;
    }

    public static function to()
    {
        $to = Utils::checkInteger(Utils::queryParam('to', time()), 'to');
        if ($to < 0)
        {
            throw new ResponseException("query parameter 'to' must be non-negative",
                        Response::BADREQUEST);
        }
        return $to;
    }

    public static function page()
    {
        $page = Utils::checkInteger(Utils::queryParam('page', "1"), 'page');
        if ($page <= 0)
        {
            throw new ResponseException('query parameter page must be positive',
                        Response::BADREQUEST);
        }
        return $page;
    }

    public static function count()
    {
        $count = Utils::checkInteger(Utils::queryParam('count', "50"), 'count');
        if ($count <= 0)
        {
            throw new ResponseException('query parameter count must be positive',
                        Response::BADREQUEST);
        }
        return $count;
    }
}

