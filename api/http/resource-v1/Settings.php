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

    function post($request)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        if (cfapi_settings_post($user, $request->data))
        {
            $response->code = Response::NOCONTENT;
        }
        else
        {
            $response->code = Response::INTERNALSERVERERROR;
        }

        return $response;
    }
}