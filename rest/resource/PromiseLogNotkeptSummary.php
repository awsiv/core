<?php

/**
 * @uri /promise/log/not-kept/summary
 */
class PromiseLogNotKeptSummary extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'handle', 'context', 'to'));

        $username = $_SERVER['PHP_AUTH_USER'];
        $hostkey = Utils::queryParam('hostkey');
        $handle = Utils::queryParam('handle');
        $context = Utils::queryParam('context');
        $to = Utils::checkInteger(Utils::queryParam('to'), 'to');

        try
        {
            $response = new Response($request);
            $response->body = cfmod_resource_promise_log_notkept_summary($username,
                    $handle, $hostkey, $context, $to, DefaultParameters::from());
            return $response;
        }
        catch (Exception $e)
        {
            throw new ResponseException($e->getMessage(), $e->getCode());
        }
    }
}

