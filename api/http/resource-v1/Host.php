<?php

/**
 * @uri /host/([^\/]+)/vital/([^\/]+) 4
 */
class HostVital extends Resource
{
    function get($request, $username, $vital_id)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $payload = cfapi_host_vital_get($user, $username, $vital_id,
            DefaultParameters::from(), DefaultParameters::to());
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
 * @uri /host/(.+)/context 3
 */
class HostContextList extends Resource
{
    function get($request, $hostid)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $payload = cfapi_host_context_list($user, $hostid);
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
 * @uri /host/(.+)/vital 2
 */
class HostVitalList extends Resource
{
    function get($request, $hostid)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $payload = cfapi_host_vital_list($user, $hostid);
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
 * @uri /host/:id 1
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
 * @uri /host 0
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