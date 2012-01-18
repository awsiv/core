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
        
        $hostkey = Utils::queryParam('hostkey');
        $name = Utils::queryParam('name');
        $version = Utils::queryParam('version');
        $arch = Utils::queryParam('arch');
        $context = Utils::queryParam('context');
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');
        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');
        $to = Utils::checkInteger(Utils::queryParam('to'), 'to');
        $scope = Utils::queryParam('scope');
        $value = Utils::queryParam('value');
        $kept = Utils::queryParam('kept');
        $notkept = Utils::queryParam('notkept');
        $repaired = Utils::queryParam('repaired');
        $handle = Utils::queryParam('handle');
        $status = Utils::queryParam('status'); // TODO: discontinue
        $job = Utils::queryParam('job');

        try
        {
            if ($id == '')
            {
                $response->body = cfmod_resource_report_list();
                return $response;
            }
            else
            {
                switch ($id)
                {
                    case 'bundle-profile':
                        $response->body = cfmod_resource_report_bundle_profile($hostkey,
                                $name, $context, $count, $startPage);
                        break;

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

                    case 'compliance-summary':
                        $response->body = cfpr_report_compliance_summary($hostkey,
                                NULL, $from, $kept, $notkept, $repaired, ">",
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
                        $response->body = cfmod_resource_report_setuid_programs($hostkey,
                                $name, $context, $count, $startPage);
                        break;

                    case 'file-change-log':
                        $response->body = cfpr_report_filechanges($hostkey, $name,
                                true, $from, ">", $context, $count, $startPage);
                        break;

                    case 'file-change-diffs':
                        $response->body = cfpr_report_filediffs($hostkey, $name,
                                $value, true, $from, ">", $context, $count,
                                $startPage);
                        break;

                    default:
                        $response->code = Response::NOTFOUND;
                        return $response;
                }
                return $response;
            }
        }
        catch (Exception $e)
        {
            return Utils::InternalExceptionResponse($request, $e);
        }
    }
}
