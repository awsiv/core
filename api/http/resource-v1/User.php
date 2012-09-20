<?php

/**
 * @uri /user/([^\/]+)/subscription/query/([^\/]+) 3
 */
class UserSubscriptionQuery extends Resource
{
    function get($request, $username, $subscription_id)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);

        $payload = cfapi_user_subscription_query_get($user, $username, $subscription_id);
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

    function put($request, $username, $subscription_id)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $data = json_decode($request->data);

        $response = new Response($request);

        if (cfapi_user_subscription_query_put($user, $username, $subscription_id,
                $data->to, $data->enabled, $data->query, $data->schedule,
                $data->title, $data->description))
        {
            $response->code = Response::CREATED;
        }
        else
        {
            $response->code = Response::INTERNALSERVERERROR;
        }

        return $response;
    }

    function delete($request, $username, $subscription_id)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);

        if (cfapi_user_subscription_query_delete($user, $username, $subscription_id))
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
 * @uri /user/(.+)/subscription/query 2
 */
class UserSubscriptionQueryList extends Resource
{
    function get($request, $username)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $payload = cfapi_user_subscription_query_list($user, $username);
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
}

/**
 * @uri /user/:username 1
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

        $response = new Response($request);

        if (cfapi_user_put($user, $username, $data->password,
                $data->name, $data->email, $data->roles))
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

        $response = new Response($request);

        if (cfapi_user_post($user, $username, $data->password,
                $data->name, $data->email, $data->roles))
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
 * @uri /user 0
 */
class UserList extends Resource
{
    function get($request)
    {
        $user = $_SERVER['PHP_AUTH_USER'];
        $pass = $_SERVER['PHP_AUTH_PW'];

        $external = Utils::checkBoolean(Utils::queryParam('external'), 'external');

        $response = new Response($request);
        $response->body = cfapi_user_list($user, $external,
                DefaultParameters::page(),
                DefaultParameters::count());
        $response->code = Response::OK;

        return $response;
    }
}
