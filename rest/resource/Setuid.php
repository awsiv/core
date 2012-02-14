<?php

/**
 * @uri /setuid
 */
class Setuid extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'name', 'context'));

        $hostkey = Utils::queryParam('hostkey');
        $name = Utils::queryParam('name');
        $context = Utils::queryParam('context');

        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        $response = new Response($request);
        $payload = cfmod_resource_setuid($hostkey, $name, $context,
                $count, $startPage);

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
