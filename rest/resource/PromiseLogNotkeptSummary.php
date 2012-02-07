<?php

/**
 * @uri /promise/log/not-kept/summary
 */
class PromiseLogNotKeptSummary extends Resource
{
    function get($request)
    {   
        $hostkey = Utils::queryParam('hostkey');
        $handle = Utils::queryParam('handle');
        $context = Utils::queryParam('context');
        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');
        $to = Utils::checkInteger(Utils::queryParam('to'), 'to');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        try
        {
            $response = new Response($request);
            $response->body = cfmod_resource_promise_log_notkept_summary($handle,
                    $hostkey, $context, $from, $to, $count, $startPage);
            return $response;
        }
        catch (Exception $e)
        {
            throw new ResponseException($e->getMessage(), $e->getCode());
        }
    }
}

