<?php

/**
 * @uri /host/:id/seen-by 2
 */
class HostIdSeenBy extends Resource
{
    function get($request, $id)
    {
        Utils::checkValidQueryParams(array(
            'context', 'from'
        ));

        $context = Utils::queryParam('context');
        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');
        $sort = Utils::queryParam('sort');
        $order = Utils::checkBoolean(Utils::queryParam('order'), 'order');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        $response = new Response($request);
        $payload = cfmod_resource_host_id_seen_by($id, $context,
                $from, $sort, $order, $count, $startPage);
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