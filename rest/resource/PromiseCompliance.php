<?php

/**
 * @uri /promise/compliance
 */
class PromiseCompliance extends Resource
{
    function get($request)
    {   
        $hostkey = Utils::queryParam('hostkey');
        $handle = Utils::queryParam('handle');
        $context = Utils::queryParam('context');
        $state = Utils::queryParam('context');
        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');
        $sort = Utils::queryParam('sort');
        $order = Utils::checkBoolean(Utils::queryParam('order'), 'order');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');
        
        try
        {
            $response = new Response($request);
            $response->body = cfmod_resource_promise_compliance($handle, $hostkey,
                    $context, $state, $from,
                    $sort, $order, $count, $startPage);
            return $response;
        }
        catch (Exception $e)
        {
            throw new ResponseException($e->getMessage(), $e->getCode());
        }
    }
}
