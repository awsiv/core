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
        $response->body = cfapi_role_get($user, $rolename);
        $response->code = Response::OK;

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
