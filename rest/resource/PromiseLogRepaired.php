<?php

/**
 * @uri /promise/log/repaired/
 */
class PromiseLogRepaired extends Resource
{
    function get($request, $report_id, $state, $is_summary)
    {   
        $hostkey = Utils::queryParam('hostkey');
        $handle = Utils::queryParam('handle');
        $context = Utils::queryParam('context');
        $to = Utils::checkInteger(Utils::queryParam('to'), 'to');

        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        try
        {
            $response = new Response($request);
            $response->body = cfmod_resource_promise_log_repaired($handle,
                    $hostkey, $context, $to, $from,
                    $count, $startPage);
            return $response;
        }
        catch (Exception $e)
        {
            throw new ResponseException($e->getMessage(), $e->getCode());
        }
    }
}
