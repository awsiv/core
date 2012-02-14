<?php

/**
 * @uri /promise/compliance
 */
class PromiseCompliance extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array(
            'hostkey', 'handle', 'context', 'state'
        ));
        
        $hostkey = Utils::queryParam('hostkey');
        $handle = Utils::queryParam('handle');
        $context = Utils::queryParam('context');
        $state = Utils::checkPromiseState(Utils::queryParam('state'));

        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');
        
        try
        {
            $response = new Response($request);
            $response->body = cfmod_resource_promise_compliance($handle, $hostkey,
                    $context, $state, $from,
                    $count, $startPage);
            return $response;
        }
        catch (Exception $e)
        {
            throw new ResponseException($e->getMessage(), $e->getCode());
        }
    }
}
