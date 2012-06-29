<?php

require_once 'lib/Tonic.php';
require_once 'lib/Utils.php';
require_once 'lib/DefaultParameters.php';

// load resources
require_once 'resource-v1/Query.php';
require_once 'resource-v1/Status.php';
require_once 'resource-v1/User.php';



$request = new Request(array(
        'baseUri' => '/api',
        'mimetypes' => array(
            'json-v1' => 'application/vnd.cfengine.enterprise-v1+json'
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
        !cfapi_auth($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW']))
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
        break;

    case Response::INTERNALSERVERERROR:
        $response = $e->response($request);
        break;

    default:
        $response = $e->response($request);
    }
}
catch (CFAPIExceptionAccess $e)
{
    $response = new Response($request);
    $response->body = $e->getMessage();
    $response->code = Response::FORBIDDEN;
}
catch (Exception $e)
{
    $response = new Response($request);
    $response->body = $e->getMessage();
    $response->code = Response::INTERNALSERVERERROR;
}

$response->addHeader('Content-type', 'application/vnd.cfengine.enterprise-v1+json');
$response->output();
return;
