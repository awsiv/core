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
        
        $hostkey = Utils::queryParam('hostkey');
        $name = Utils::queryParam('name');
        $version = Utils::queryParam('version');
        $arch = Utils::queryParam('arch');
        $context = Utils::queryParam('context');
        $count = Utils::queryParam('count');
        $startPage = Utils::queryParam('startPage');
        $since = Utils::queryParam('since');
        $scope = Utils::queryParam('scope');
        $value = Utils::queryParam('value');
        $kept = Utils::queryParam('kept');
        $notkept = Utils::queryParam('notkept');
        $repaired = Utils::queryParam('repaired');

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
                    $response->body = cfpr_report_software_in($hostkey,
                            $name, $version, $arch, true, $context, $count,
                            $startPage);
                    break;

                case 'patches-installed':
                    $response->body = cfpr_report_patch_in($hostkey,
                            $name, $version, $arch, true, $context, $count,
                            $startPage);
                    break;

                case 'patches-available':
                    $response->body = cfpr_report_patch_avail($hostkey,
                            $name, $version, $arch, true, $context, $count,
                            $startPage);
                    break;

                case 'contexts':
                    $response->body = cfpr_report_classes($hostkey, NULL, true,
                            $context, $count, $startPage);
                    break;

                case 'values':
                    $response->body = cfpr_report_vars($hostkey, $scope, $name,
                            $value, $type, true, $context, $count, $startPage);
                    break;

                case 'compliance-summary':
                    $response->body = cfpr_report_compliance_summary($hostkey,
                            NULL, $since, $kept, $notkept, $repaired, ">",
                            $context, $count, $startPage);
                    break;

                default:
                    $response->code = Response::NOTFOUND;
                    return $response;
            }

            return $response;
        }
    }
}
