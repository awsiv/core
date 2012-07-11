<?php

/**
 * @uri /role/:rolename 0
 */
class Role extends Resource
{
    function get($request, $rolename)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $payload = cfapi_role_get($user, $rolename);
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

    function put($request, $rolename)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $data = json_decode($request->data);

        $response = new Response($request);
        if (cfapi_role_put($user, $rolename, $data->description,
                $data->includeContext, $data->excludeContext,
                $data->includeBundles, $data->excludeBundles))
        {
            $response->code = Response::CREATED;
        }
        else
        {
            $response->code = Response::INTERNALSERVERERROR;
        }

        return $response;
    }

    function post($request, $rolename)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $data = json_decode($request->data);

        $response = new Response($request);
        if (cfapi_role_post($user, $rolename, $data->description,
                $data->includeContext, $data->excludeContext,
                $data->includeBundles, $data->excludeBundles))
        {
            $response->code = Response::NOCONTENT;
        }
        else
        {
            $response->code = Response::INTERNALSERVERERROR;
        }

        return $response;
    }

    function delete($request, $rolename)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);

        if (cfapi_role_delete($user, $rolename))
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
 * @uri /role 1
 */
class RoleList extends Resource
{
    function get($request)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $response->body = cfapi_role_list($user);
        $response->code = Response::OK;

        return $response;
    }
}
