<?php

require_once 'lib/tonic.php';
require_once 'lib/utils.php';

// load resources
require_once 'resource/context.php';
require_once 'resource/host-id-seen.php';
require_once 'resource/host-id-seenby.php';
require_once 'resource/host-id.php';
require_once 'resource/host.php';
require_once 'resource/promise-compliance.php';
require_once 'resource/promise-log-notkept-summary.php';
require_once 'resource/promise-log-notkept.php';
require_once 'resource/promise-log-repaired-summary.php';
require_once 'resource/promise-log-repaired.php';
require_once 'resource/setuid.php';
require_once 'resource/software.php';
require_once 'resource/variable.php';

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
        //throw new ResponseException("Not authenticated", Response::UNAUTHORIZED);
    }

    $resource = $request->loadResource();
    $response = $resource->exec($request);

    $response->addHeader('Content-type', 'application/json');
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
}
catch (Exception $e)
{
    $response = Utils::InternalExceptionResponse($request, $e);
}

$response->output();
return;
