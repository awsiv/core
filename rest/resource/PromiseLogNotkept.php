<?php

/**
 * @uri /promise/log/notkept/
 */
class PromiseLogNotKept extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'handle', 'context',
            'cause', 'to'));

        $username = $_SERVER['PHP_AUTH_USER'];
        $hostkey = Utils::queryParam('hostkey');
        $handle = Utils::queryParam('handle');
        $context = Utils::queryParam('context');
        $cause = Utils::queryParam('cause');
        $to = Utils::checkInteger(Utils::queryParam('to'), 'to');

        try
        {
            $response = new Response($request);
            $response->body = cfmod_resource_promise_log_notkept($username,
                    $handle, $cause, $hostkey, $context, $to, DefaultParameters::from());
            return $response;
        }
        catch (Exception $e)
        {
            throw new ResponseException($e->getMessage(), $e->getCode());
        }
    }
}