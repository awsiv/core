<?php

/**
 * @uri /file
 */
class File extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'path', 'context'));

        $username = $_SERVER['PHP_AUTH_USER'];
        $hostkey = Utils::queryParam('hostkey');
        $path = Utils::queryParam('path');
        $context = Utils::queryParam('context');

        $response = new Response($request);
        $payload = cfmod_resource_file($username,
                $hostkey, $path, $context,
                DefaultParameters::from(), DefaultParameters::to(),
                DefaultParameters::page(), DefaultParameters::count());

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

