<?php


class ResponsePackaging
{    
    public static function package($response)
    {
        switch ($response->code)
        {
            case Response::OK:
                ResponsePackaging::ok($response);
                break;
            
            default:
                ResponsePackaging::error($response);
                break;
        }
        
        return $response;
    }
    
    private static function error($response)
    {
        $package = array(
            'status' => 'error',
            'timestamp' => time(),
            'message' => $response->body
        );
        
        $response->body = json_encode($package);
    }
    
    private static function ok($response)
    {
        $payload = json_decode($response->body);
        if (is_null($payload))
        {
            $response->body = 'Could not decode JSON payload from subsystem';
            $response->code = Response::INTERNALSERVERERROR;
            return ResponsePackaging::package($response);
        }
        
        $count = DefaultParameters::count();
        $page = DefaultParameters::page();
        $start = $count * ($page - 1);
        
        $total = 1;
        if (is_array($payload))
        {
            $total = sizeof($payload);
            array_slice($payload, $start, $count);
        }
        
        $package = array(
            'status' => 'ok',
            'timestamp' => time(),
            'page' => $page,
            'count' => $count,
            'total' => $total,
            'result' => $payload
        );
        $response->body = json_encode($package);
    }
}

