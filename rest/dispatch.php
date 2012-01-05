<?php

// load Tonic library
require_once 'lib/tonic.php';

// load resources
require_once 'resource/host.php';
require_once 'resource/report.php';

// handle request
$request = new Request();
try
{
    $resource = $request->loadResource();
    $response = $resource->exec($request);
} 
catch (ResponseException $e) 
{
    switch ($e->getCode()) {
    case Response::UNAUTHORIZED:
        $response = $e->response($request);
        $response->addHeader('WWW-Authenticate', 'Basic realm="CFEngine Nova"');
        break;
    default:
        $response = $e->response($request);
    }
}
$response->output();

