<?php

require_once 'lib/Tonic.php';
require_once 'lib/Utils.php';
require_once 'lib/DefaultParameters.php';
require_once 'lib/ResponsePackaging.php';

// load resources
require_once 'resource/Context.php';
require_once 'resource/File.php';
require_once 'resource/HostIdSeen.php';
require_once 'resource/HostIdSeenby.php';
require_once 'resource/HostId.php';
require_once 'resource/Host.php';
require_once 'resource/PromiseCompliance.php';
require_once 'resource/PromiseLogNotkeptSummary.php';
require_once 'resource/PromiseLogNotkept.php';
require_once 'resource/PromiseLogRepairedSummary.php';
require_once 'resource/PromiseLogRepaired.php';
require_once 'resource/Setuid.php';
require_once 'resource/Software.php';
require_once 'resource/Status.php';
require_once 'resource/Variable.php';

function addAuthenticateHeader($response)
{
    $response->addHeader('WWW-Authenticate', 'Basic realm="CFEngine Nova"');
}

$request = new Request(array(
        'baseUri' => '/rest',
        'mimetypes' => array(
            'json-v1' => 'application/vnd.cfengine.nova-v1+json'
        )
));

try
{
    $format = DefaultParameters::format();
    if (!empty($request->accept))
    {
        $format = $request->mostAcceptable(array(DefaultParameters::format()));
    }
    
    switch ($format)
    {
        case 'json-v1':
            break;
        
        default:
            throw new ResponseException("Unsupported format or version requested",
                    Response::NOTACCEPTABLE);
    }
    
    if (!isset($_SERVER['PHP_AUTH_USER']) || !isset($_SERVER['PHP_AUTH_PW']) ||
        !cfpr_user_authenticate($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']))
    {
        throw new ResponseException("Not authenticated", Response::UNAUTHORIZED);
    }

    DefaultParameters::check();

    $resource = $request->loadResource();
    $response = $resource->exec($request);
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

ResponsePackaging::package($response)->output();
return;
