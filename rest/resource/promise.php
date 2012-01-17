<?php

/**
 * @uri /promise/:report-id/:state/:summary
 * @uri /promise/:report-id/:state
 * @uri /promise/:report-id
 */
class Promise extends Resource
{
    function get($request, $report_id, $state, $is_summary)
    {
        $hostkey = Utils::queryParam('hostkey');
        $handle = Utils::queryParam('handle');
        $context = Utils::queryParam('context');

        $from = Utils::checkInteger(Utils::queryParam('from'), 'from');
        $to = Utils::checkInteger(Utils::queryParam('to'), 'to');
        
        $count = Utils::checkInteger(Utils::queryParam('count'), 'count');
        $startPage = Utils::checkInteger(Utils::queryParam('startPage'), 'startPage');

        try
        {
            switch ($report_id)
            {
                case 'log':
                    $response = $this->promise_log_response($request, $state,
                            $is_summary, $handle, $hostkey, $context, $from,
                            $to, $count, $startPage);
                    $response->code = Response::OK;
                    return $response;
                
                case 'compliance':
                    $response = new Response($request);
                    $response->body = cfmod_resource_promise_compliance($handle,
                            $hostkey, $context, $state, $from, $count, $startPage);
                    return $response;
                
                default:
                    throw new ResponseException("Unknown report id: " . $report_id, 
                            Response::NOTFOUND);
            }
        }
        catch (CFModExceptionArgs $ae)
        {
            throw new ResponseException($ae->getMessage(), $ae->getCode());
        }
        catch (CFModExceptionDB $dbe)
        {
            throw new ResponseException($dbe->getMessage(), $dbe->getCode());
        }
    }
    
    function promise_log_response($request, $state, $is_summary,
            $handle, $hostkey, $context, $from, $to, $count, $startPage)
    {
        $response = new Response($request);

        switch ($state)
        {
            case 'repaired':
                if ($is_summary)
                {
                    $response->body = cfmod_resource_promise_log_repaired_summary($handle,
                            $hostkey, $context, $from, $to, $count, $startPage);
                }
                else
                {
                    $response->body = cfmod_resource_promise_log_repaired($handle,
                            $hostkey, $context, $from, $to, $count, $startPage);
                }
                break;

            case 'not-kept':
                if ($is_summary)
                {
                    $response->body = cfmod_resource_promise_log_notkept_summary($handle,
                            $hostkey, $context, $from, $to, $count, $startPage);
                }
                else
                {
                    $response->body = cfmod_resource_promise_log_notkept($handle,
                            $hostkey, $context, $from, $to, $count, $startPage);
                }
                break;

            default:
                throw new ResponseException("Unknown promise log type", Response::NOTFOUND);
        }

        return $response;
    }

}

?>
