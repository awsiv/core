<?php

/**
 * @uri /software
 */
class Software extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'name', 'version',
            'arch', 'context'));

        $username = $_SERVER['PHP_AUTH_USER'];
        $hostkey = Utils::queryParam('hostkey');
        $name = Utils::queryParam('name');
        $version = Utils::queryParam('version');
        $arch = Utils::queryParam('arch');
        $context = Utils::queryParam('context');

        $response = new Response($request);
        $payload = cfmod_resource_software($username,
                $hostkey, $name, $version, $arch, $context);

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
