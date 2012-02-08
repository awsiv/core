<?php

/**
 * @uri /setuid
 */
class Setuid extends Resource
{
    function get($request)
    {
        $hostkey = Utils::queryParam('hostkey');
        $name = Utils::queryParam('name');
        $context = Utils::queryParam('context');
        $sort = Utils::queryParam('sort');
        $order = Utils::checkBoolean(Utils::queryParam('order'), 'order');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        $response = new Response($request);
        $payload = cfmod_resource_setuid($hostkey, $name, $context,
                $sort, $order, $count, $startPage);

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
