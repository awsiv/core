<?php

/**
 * @uri /context
 */
class Context extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'context'));
        
        $username = $_SERVER['PHP_AUTH_USER'];
        $hostkey = Utils::queryParam('hostkey');
        $context = Utils::queryParam('context');

        $response = new Response($request);
        $payload = cfmod_resource_context($username,
                $hostkey, $context, DefaultParameters::from());

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
