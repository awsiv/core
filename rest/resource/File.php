<?php

/**
 * @uri /file
 */
class File extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'path', 'context'));
        
        $hostkey = Utils::queryParam('hostkey');
        $path = Utils::queryParam('path');
        $context = Utils::queryParam('context');

        $response = new Response($request);
        $payload = cfmod_resource_file($hostkey, $path, $context, DefaultParameters::from());

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

