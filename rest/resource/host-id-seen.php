<?php

/**
 * @uri /host/:id/seen 3
 */
class HostIdSeen extends Resource
{
    function get($request, $id)
    {
        $remote_name = Utils::queryParam('remote-name');
        $remote_ip = Utils::queryParam('remote-ip');
        $context = Utils::queryParam('context');
        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        $response = new Response($request);
        $payload = cfmod_resource_host_id_seen($id, $rename_name, $remote_ip, $context,
                $from, $count, $startPage);
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