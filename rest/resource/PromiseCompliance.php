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

        $username = $_SERVER['PHP_AUTH_USER'];
        $hostkey = Utils::queryParam('hostkey');
        $handle = Utils::queryParam('handle');
        $context = Utils::queryParam('context');
        $state = Utils::checkPromiseState(Utils::queryParam('state'));

        try
        {
            $response = new Response($request);
            $response->body = cfmod_resource_promise_compliance($username,
                    $handle, $hostkey, $context, $state,
                    DefaultParameters::from(), DefaultParameters::to());
            return $response;
        }
        catch (Exception $e)
        {
            throw new ResponseException($e->getMessage(), $e->getCode());
        }
    }
}
