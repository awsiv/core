<?php

/**
 * @uri /variable
 */
class Variable extends Resource
{
    function get($request)
    {
        Utils::checkValidQueryParams(array('hostkey', 'scope', 'name', 'value',
            'type'));

        $hostkey = Utils::queryParam('hostkey');
        $scope = Utils::queryParam('scope');
        $name = Utils::queryParam('name');
        $value = Utils::queryParam('value');
        $type = Utils::queryParam('type');

        $response = new Response($request);
        $payload = $response->body = cfmod_resource_variable($hostkey, 
                $scope, $name, $value, $type, $context);
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