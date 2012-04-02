<?php

/**
 * @uri /host/:id/seen-by 2
 */
class HostIdSeenBy extends Resource
{
    function get($request, $id)
    {
        Utils::checkValidQueryParams(array());

        $username = $_SERVER['PHP_AUTH_USER'];

        $response = new Response($request);
        $payload = cfmod_resource_host_id_seenby($username,
                $id, DefaultParameters::from(), DefaultParameters::to());
        if (is_null($payload))
        {
            $response = new Response($request);
            $response->code = Response::NOTFOUND;
            $response->body = $id;
            return $response;
        }

        $response->body = $payload;
        $response->code = Response::OK;

        return $response;
    }
}