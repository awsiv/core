<?php

/**
 * @uri /user/:username 0
 */
class User extends Resource
{
    function get($request, $username)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $payload = cfapi_user_get($user, $username);
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

    function put($request, $username)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $data = json_decode($request->data);
        $roles = $data->roles ? $data->roles : array();

        $response = new Response($request);

        if (cfapi_user_put($user, $username, $data->password, $data->active,
                $data->email, $roles))
        {
            $response->code = Response::CREATED;
        }
        else
        {
            $response->code = Response::INTERNALSERVERERROR;
        }

        return $response;
    }

    function post($request, $username)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $data = json_decode($request->data);
        $roles = $data->roles ? $data->roles : array();

        $response = new Response($request);

        if (cfapi_user_post($user, $username, $data->password, $data->active,
                $data->email, $roles))
        {
            $response->code = Response::NOCONTENT;
        }
        else
        {
            $response->code = Response::INTERNALSERVERERROR;
        }

        return $response;
    }

    function delete($request, $username)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);

        if (cfapi_user_delete($user, $username))
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

/**
 * @uri /user 1
 */
class UserList extends Resource
{
    function get($request)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $response->body = cfapi_user_list($user);
        $response->code = Response::OK;

        return $response;
    }
}