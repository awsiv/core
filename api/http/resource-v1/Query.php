<?php

/**
 * @uri /query
 */
class Query extends Resource
{
    function post($request)
    {
        $username = $_SERVER['PHP_AUTH_USER'];
        $query = file_get_contents("php://input");

        $response = new Response($request);
        $response->body = cfapi_query($username, $query);
        $response->code = Response::OK;

        return $response;
    }
}
