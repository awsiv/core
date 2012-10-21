<?php

/**
 * @uri /query/async/:token 2
 */
class QueryAsyncId extends Resource
{
    function get($request, $token)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);

        $static_files_uri = 'sbc';

        $payload = cfapi_query_async_get($user, $token, $static_files_uri);
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

    function delete($request, $token)
    {
        $user = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);

        if (cfapi_query_async_delete($user, $token))
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
 * @uri /query/async 1
 */
class QueryAsync extends Resource
{
    function post($request)
    {
        $username = $_SERVER['PHP_AUTH_USER'];

        $data = json_decode($request->data);

        if (!$data->query)
        {
            $response = new Response($request);
            $response->code = Response::BADREQUEST;
            $response->body = "'query' field is required";
            return $response;
        }

        $response = new Response($request);
        $response->body = cfapi_query_async_post($username, $data->query);
        $response->code = Response::OK;

        return $response;
    }
}

/**
 * @uri /query 0
 */
class Query extends Resource
{
    function post($request)
    {
        $username = $_SERVER['PHP_AUTH_USER'];

        $data = json_decode($request->data);

        if (!$data->query)
        {
            $response = new Response($request);
            $response->code = Response::BADREQUEST;
            $response->body = "'query' field is required";
            return $response;
        }

        if (!$data->sortDescending)
        {
            $data->sortDescending = false;
        }

        if ($data->skip)
        {
            if (gettype($data->skip) !== "integer")
            {
                $response = new Response($request);
                $response->code = Response::BADREQUEST;
                $response->body = "'skip' field must be an integer";
                return $response;
            }
        }
        else
        {
            $data->skip = -1;
        }

        if ($data->limit)
        {
            if (gettype($data->limit) !== "integer")
            {
                $response = new Response($request);
                $response->code = Response::BADREQUEST;
                $response->body = "'limit' field must be an integer";
                return $response;
            }
        }
        else
        {
            $data->limit = -1;
        }


        $response = new Response($request);
        $response->body = cfapi_query_post($username,
                                           $data->query,
                                           $data->sortColumn,
                                           $data->sortDescending,
                                           $data->skip,
                                           $data->limit);
        $response->code = Response::OK;

        return $response;
    }
}
