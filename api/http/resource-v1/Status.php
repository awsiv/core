<?php

/**
 * @uri /
 */
class Status extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array());

        $response = new Response($request);
        $response->body = cfapi();
        $response->code = Response::OK;

        return $response;
    }
}
