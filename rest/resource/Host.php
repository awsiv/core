<?php

/**
 * @uri /host 0
 */
class Host extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostname', 'ip', 'context'));

        $hostname = Utils::queryParam('hostname');
        $ip = Utils::queryParam('ip');
        $context = Utils::queryParam('context');

        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        $response = new Response($request);
        $response->body = cfmod_resource_host($hostname, $ip, $count, $startPage);
        $response->code = Response::OK;

        return $response;
    }
}
