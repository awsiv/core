<?php

/**
 * @uri /context
 */
class Context extends Resource
{
    function get($request)
    {
        $hostkey = Utils::queryParam('hostkey');
        $context = Utils::queryParam('context');
        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $sort = Utils::queryParam('sort');
        $order = Utils::checkBoolean(Utils::queryParam('order'), 'order');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        $response = new Response($request);
        $payload = cfmod_resource_context($hostkey, $context, $from, 
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
