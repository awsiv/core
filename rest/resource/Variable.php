<?php

/**
 * @uri /variable
 */
class Variable extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'scope', 'name', 'value',
            'type', 'context'));

        $username = $_SERVER['PHP_AUTH_USER'];
        $hostkey = Utils::queryParam('hostkey');
        $scope = Utils::queryParam('scope');
        $name = Utils::queryParam('name');
        $value = Utils::queryParam('value');
        $type = Utils::queryParam('type');
        $context = Utils::queryParam('context');

        $response = new Response($request);
        $payload = $response->body = cfmod_resource_variable($username,
                $hostkey, $scope, $name, $value, $type, $context,
                DefaultParameters::from(), DefaultParameters::to(),
                DefaultParameters::page(), DefaultParameters::count());
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