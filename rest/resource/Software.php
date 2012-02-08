<?php

/**
 * @uri /software
 */
class Software extends Resource
{
    function get($request)
    {
        $hostkey = Utils::queryParam('hostkey');
        $name = Utils::queryParam('name');
        $version = Utils::queryParam('version');
        $arch = Utils::queryParam('arch');
        $context = Utils::queryParam('context');
        $sort = Utils::queryParam('sort');
        $order = Utils::checkBoolean(Utils::queryParam('order'), 'order');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        $response = new Response($request);
        $payload = cfmod_resource_software($hostkey, $name, $version, $arch,
                $context, $sort, $order, $count, $startPage);

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
