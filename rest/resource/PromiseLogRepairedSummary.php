<?php

/**
 * @uri /promise/log/repaired/summary
 */
class PromiseLogRepairedSummary extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'handle', 'context', 'to'));

        $hostkey = Utils::queryParam('hostkey');
        $handle = Utils::queryParam('handle');
        $context = Utils::queryParam('context');
        $to = Utils::checkInteger(Utils::queryParam('to'), 'to');

        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');

        try
        {
            $response = new Response($request);
            $response->body = cfmod_resource_promise_log_repaired_summary($handle,
                    $hostkey, $context, $to, $from);
            return $response;
        }
        catch (Exception $e)
        {
            throw new ResponseException($e->getMessage(), $e->getCode());
        }
    }
}
