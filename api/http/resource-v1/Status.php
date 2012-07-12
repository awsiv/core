<?php

/**
 * @uri /
 */
class Status extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array());

        $user = $_SERVER['PHP_AUTH_USER'];
        $pass = $_SERVER['PHP_AUTH_PW'];

        $response = new Response($request);
        $response->body = cfapi($user, $pass);
        $response->code = Response::OK;

        return $response;
    }
}
