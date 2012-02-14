<?php

/**
 * @uri /context
 */
class Context extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'context'));
        
        $hostkey = Utils::queryParam('hostkey');
        $context = Utils::queryParam('context');

        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        $response = new Response($request);
        $payload = cfmod_resource_context($hostkey, $context, $from, 
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
