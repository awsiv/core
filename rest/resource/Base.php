<?php

/**
 * @uri /
 */
class Base extends Resource
{
    function get($request)
    {
        $response = new Response($request);
        $response->body = cfmod_resource();
        $response->code = Response::OK;

        return $response;
    }
}
