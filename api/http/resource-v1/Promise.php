<?php

/**
 * @uri /promise 0
 */
class PromiseList extends Resource
{
    function get($request)
    {
        $username = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);

        $type = Utils::queryParam('type');

        $response->body = cfapi_promise_list($username,
                $type,
                DefaultParameters::page(),
                DefaultParameters::count());
        $response->code = Response::OK;

        return $response;
    }
}

