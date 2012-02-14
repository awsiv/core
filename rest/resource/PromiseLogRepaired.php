<?php

/**
 * @uri /promise/log/repaired/
 */
class PromiseLogRepaired extends Resource
{
    function get($request, $report_id, $state, $is_summary)
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
            $response->body = cfmod_resource_promise_log_repaired($handle,
                    $hostkey, $context, $to, $from);
            return $response;
        }
        catch (Exception $e)
        {
            throw new ResponseException($e->getMessage(), $e->getCode());
        }
    }
}
