<?php

/**
 * @uri /host/:id
 */
class HostId extends Resource
{
    function get($request, $id)
    {        
        $response = new Response($request);
        $payload = cfmod_resource_host_id($id);
        if (is_null($payload))
        {
            $response = new Response($request);
            $response->code = Response::NOTFOUND;
            $response->body = $id;
            return $response;
        }
        
        $response->code = Response::OK;

        return $response;
    }
}
