<?php

/**
 * @uri /host/:id 1
 */
class HostId extends Resource
{
    function get($request, $id)
    {
        Utils::checkValidQueryParams(array());

        $response = new Response($request);
        $payload = cfmod_resource_host_id($id);
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
