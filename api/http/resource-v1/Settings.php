<?php

/**
 * @uri /settings
 */
class Settings extends Resource
{
    function get($request)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $response->body = cfapi_settings_get($user);
        $response->code = Response::OK;

        return $response;
    }
}