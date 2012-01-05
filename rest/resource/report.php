<?php

/**
 * @uri /report
 */
class ReportResource extends Resource
{
    function get($request) 
    {
        $response = new Response($request);
        
        $response->code = Response::OK;
        $response->addHeader('Content-type', 'application/json');
        $response->body = cfmod_resource_report_list();
        
        return $response;
    }
}
