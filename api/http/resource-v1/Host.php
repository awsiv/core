<?php

/**
 * @uri /host/:id 0
 */
class Host extends Resource
{
    function get($request, $id)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $payload = cfapi_host_get($user, $id);
        if ($payload)
        {
            $response->code = Response::OK;
            $response->body = $payload;
        }
        else
        {
            $response->code = Response::NOTFOUND;
        }

        return $response;
    }
}

/**
 * @uri /host 1
 */
class HostList extends Resource
{
    function get($request)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $context_include = array();
        if (Utils::queryParam('context-include'))
        {
            $context_include = explode(',', Utils::queryParam('context-include'));
        }

        $context_exclude = array();
        if (Utils::queryParam('context-exclude'))
        {
            $context_exclude = explode(',', Utils::queryParam('context-exclude'));
        }

        $response = new Response($request);
        $response->body = cfapi_host_list($user,
                $context_include,
                $context_exclude,
                DefaultParameters::page(), DefaultParameters::count());
        $response->code = Response::OK;

        return $response;
    }
}