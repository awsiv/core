<?php

/**
 * @uri /report
 * @uri /report/:id
 */
class ReportResource extends Resource
{
    function get($request, $id)
    {
        $response = new Response($request);

        $response->code = Response::OK;
        $response->addHeader('Content-type', 'application/json');
        
        if ($id == '')
        {
            $response->body = cfmod_resource_report_list();
            return $response;
        }
        else
        {
            switch ($id)
            {
                case 'software-installed':
                    $response->body = cfpr_report_software_in($_GET['hostkey'],
                            $_GET['name'],
                            $_GET['version'],
                            $_GET['arch'],
                            1,
                            $_GET['context'],
                            $_GET['count'],
                            $_GET['startPage']);
                    break;

                case 'contexts':
                    $response->body = cfpr_report_classes($_GET['hostkey'],
                            NULL,
                            1,
                            $_GET['context'],
                            $_GET['count'],
                            $_GET['startPage']);
                    break;

                case 'values':
                    $response->body = cfpr_report_vars($_GET['hostkey'],
                            $_GET['scope'],
                            $_GET['name'],
                            $_GET['value'],
                            $_GET['type'],
                            true,
                            $_GET['context'],
                            $_GET['count'],
                            $_GET['startPage']);
                    break;

                default:
                    $response->code = Response::NOTFOUND;
                    return $response;
            }

            return $response;
        }
    }
}
