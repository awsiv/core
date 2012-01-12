<?php

require_once 'lib/tonic.php';
require_once 'lib/utils.php';

// load resources
require_once 'resource/report.php';

function addAuthenticateHeader($response)
{
    $response->addHeader('WWW-Authenticate', 'Basic realm="CFEngine Nova"');
}

// handle request
$request = new Request();

try
{
    if (!isset($_SERVER['PHP_AUTH_USER']) || !isset($_SERVER['PHP_AUTH_PW']) ||
        !cfpr_user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']))
    {
        throw new ResponseException(Response::UNAUTHORIZED);
    }

    $resource = $request->loadResource();
    $response = $resource->exec($request);
    $response->output();
    return;
}
catch (ResponseException $e) 
{
    switch ($e->getCode())
    {
    case Response::UNAUTHORIZED:
        $response = $e->response($request);
        addAuthenticateHeader($response);
        break;

    case Response::INTERNALSERVERERROR:
        $response = $e->response($request);
        break;

    default:
        $response = $e->response($request);
    }

    $response->output();
    return;
}
