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

if (!isset($_SERVER['PHP_AUTH_USER']) || !isset($_SERVER['PHP_AUTH_PW']))
{
    $response = new Response($request);
    $response->code = Response::UNAUTHORIZED;
    addAuthenticateHeader($response);
    $response->output();
    return;
}

try
{
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
