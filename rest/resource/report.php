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
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');
        $since = Utils::checkInteger(Utils::queryParam('since'), 'since');
        $scope = Utils::queryParam('scope');
        $value = Utils::queryParam('value');
        $kept = Utils::queryParam('kept');
        $notkept = Utils::queryParam('notkept');
        $repaired = Utils::queryParam('repaired');
        $handle = Utils::queryParam('handle');
        $status = Utils::queryParam('status'); // TODO: discontinue
        $job = Utils::queryParam('job');

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
                    $response->body = cfmod_resource_report_software_installed($hostkey,
                            $name, $version, $arch, $context, $count, $startPage);
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
                    $response->body = cfmod_resource_report_values($hostkey, $scope, $name,
                            $value, $type, $context, $count, $startPage);
                    break;

                case 'compliance-summary':
                    $response->body = cfpr_report_compliance_summary($hostkey,
                            NULL, $since, $kept, $notkept, $repaired, ">",
                            $context, $count, $startPage);
                    break;

                case 'promise-compliance':
                    $response->body = cfpr_report_compliance_promises($hostkey,
                            $handle, $status, true, $context, $count,
                            $startPage);
                    break;

                case 'performance':
                    $response->body = cfpr_report_performance($hostkey, $job,
                            true, $context, $count, $startPage);
                    break;

                case 'setuid-programs':
                    $response->body = cfpr_report_setuid($hostkey, $name,
                            true, $context, $count, $startPage);
                    break;

                case 'file-change-log':
                    $response->body = cfpr_report_filechanges($hostkey, $name,
                            true, $since, ">", $context, $count, $startPage);
                    break;

                case 'file-change-diffs':
                    $response->body = cfpr_report_filediffs($hostkey, $name,
                            $value, true, $since, ">", $context, $count,
                            $startPage);
                    break;

                default:
                    $response->code = Response::NOTFOUND;
                    return $response;
            }

            return $response;
        }
    }
}
