<?php

/**
 * @uri /host
 */
class HostResource extends Resource
{
    function get($request) 
    {
        $response = new Response($request);
        
        $response->code = Response::OK;
        $response->addHeader('Content-type', 'application/json');
        $response->body = cfmod_resource_host_list(NULL, NULL);
        
        return $response;
    }
}

