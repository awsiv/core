<?php

/**
 * @uri /query
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
