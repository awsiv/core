<?php

/**
 * @uri /host 0
 */
class Host extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostname', 'ip', 'context'));

        $username = $_SERVER['PHP_AUTH_USER'];
        $hostname = Utils::queryParam('hostname');
        $ip = Utils::queryParam('ip');
        $context = Utils::queryParam('context');

        $response = new Response($request);
        $response->body = cfmod_resource_host($username, $hostname, $ip, $context,
                DefaultParameters::from(), DefaultParameters::to(),
                DefaultParameters::page(), DefaultParameters::count());
        $response->code = Response::OK;

        return $response;
    }
}
