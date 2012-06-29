<?php

/**
 * @uri /user/:username
 */
class User extends Resource
{
    function get($request, $username)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $response->body = cfapi_user_get($user, $username);
        $response->code = Response::OK;

        return $response;
    }

    function put($request, $username)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $data = json_decode($request->data);

        $response = new Response($request);
        $response->body = cfapi_user_put($user, $username, $data['password']);
        $response->code = Response::OK;

        return $response;
    }

    function delete($request, $username)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);

        if (cfapi_user_delete($user, $username))
        {
            $response->code = Response::OK;
        }
        else
        {
            $response->code = Response::INTERNALSERVERERROR;
        }

        return $response;
    }
}
