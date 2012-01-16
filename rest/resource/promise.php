<?php

/**
 * @uri /promise/:log/:repaired
 * @uri /promise/:log/:repaired/:summary
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
            $response = new Response($request);
            
            switch ($report_id)
            {
                case 'log':
                    switch ($state)
                    {
                        case 'repaired':
                            $response->body = cfmod_resource_promise_log_repaired($handle, $hostkey,
                                    $context, $from, $to, $count, $startPage);
                            break;

                        case 'not-kept':
                            $response->body = cfmod_resource_promise_log_notkept($handle, $hostkey,
                                    $context, $from, $to, $count, $startPage);
                            break;

                        default:
                            throw new ResponseException("Unknown promise log type", Response::NOTFOUND);
                    }
                    break;
                
                case 'compliance':
                    throw new ResponseException("Not implemented", Response::INTERNALSERVERERROR);
                    break;
                
                default:
                    throw new ResponseException("Unknown report id: " . $report_id, 
                            Response::NOTFOUND);
            }
            
            $response->code = Response::OK;
            return $response;
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
    
}

?>
