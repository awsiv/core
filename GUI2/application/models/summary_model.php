<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

class summary_model extends Cf_Model
{
    /**
     *
     * @return type
     */
    function getSummaryMeter()
    {
        try
        {
            $rawdata = cfpr_summary_meter(null);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
   function getComplianceSummaryData(){
    try
        {
            $rawdata = cfpr_compliance_summary_graph(NULL);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
       
   }
    
   function getBusinessPieChartData(){
       
        
        try
        {
            $businessValuePieData = cfpr_get_value_graph();
            $businessValuePieArray = $this->checkData($businessValuePieData);
            if (is_array($businessValuePieArray) && $this->hasErrors() == 0)
            {
              if (!empty($businessValuePieArray))
                {
                    $kept = 0;
                    $notkept = 0;
                    $repaired = 0;

                    foreach ($businessValuePieArray as $val)
                    {


                        $kept+=$val[1];
                        $notkept+=$val[3];
                        $repaired+=$val[2];
                    }
                    $data['businessValuePie']['kept'] = abs($kept);
                    $data['businessValuePie']['notkept'] = abs($notkept);
                    $data['businessValuePie']['repaired'] = abs($repaired);
                    $data['businessValuePie']['nodata'] = 0;
                }
                else
                {
                    $data['businessValuePie']['kept'] = 0;
                    $data['businessValuePie']['notkept'] = 0;
                    $data['businessValuePie']['repaired'] = 0;
                    $data['businessValuePie']['nodata'] = 100;
                }
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
       
       

       // make data ready for pie
       

   }
   
    
   function getConvertedSummaryComplianceGraphData($convertedData)
    {
        $values = array();
        $data['graphSeries'] = array();
        $labels = array('kept', 'repaired', 'not kept', 'no data');
        $count = array(); // for keeping tracks of each count
        $start = array(); // the timestamp passed of each node starttime.
        foreach ($convertedData as $key => $graphData)
        {
            $nodata = (isset($graphData['nodata'])) ? $graphData['nodata'] : 0;
            $values[] = array('label' => $graphData['title'],
                'values' => array($graphData['kept'], $graphData['repaired'], $graphData['notkept'], $nodata));

// track the count parameter
// because we cannot directly pass the custom data in barChart of infovis
            if (isset($graphData['count']))
            {
                $count[$graphData['title']] = $graphData['count'];
            }
            if (isset($graphData['start']))
            {
                $start[$graphData['title']] = $graphData['start'];
            }
        }


        $data['graphSeries']['labels'] = json_encode($labels);
        $data['graphSeries']['values'] = json_encode($values);

        // these are two extra parameters that has to be accessible in the bar chart graph
        if (is_array($count) && !empty($count))
        {
            $data['graphSeries']['count'] = json_encode($count);
        }

        if (is_array($start) && !empty($start))
        {
            $data['graphSeries']['start'] = json_encode($start);
        }
        return $data;
    }
    
    /**
     *For obtaining the status 
     * @param type $convertedData
     * @return type 
     */
    
   function getConvertedSummaryComplianceGraphStatus($convertedData)
    {
        $values = array();
        $data['graphSeries'] = array();
        $labels = array('kept', 'repaired', 'not kept', 'no data');
        $count = array(); // for keeping tracks of each count
        $start = array(); // the timestamp passed of each node starttime.
        $keptSeries = array();
        $repairedSeries = array();
        $notKeptSeries = array();
        $nodataSeries = array();
        foreach ($convertedData as $key => $graphData)
        {
            $nodata = (isset($graphData['nodata'])) ? $graphData['nodata'] : 0;
            $values[] = array('label' => $graphData['title'],
                'values' => array($graphData['kept'], $graphData['repaired'], $graphData['notkept'], $nodata));

// track the count parameter
// because we cannot directly pass the custom data in barChart of infovis
            if (isset($graphData['count']))
            {
                $count[$graphData['start']] = $graphData['count'];
            }
            if (isset($graphData['start']))
            {
                $start[$graphData['title']] = $graphData['start'];
            }

            $time = $graphData['start'] * 1000;
            $keptSeries[] = array($time, $graphData['kept']);
            $repairedSeries[] = array($time, $graphData['repaired']);
            $notKeptSeries[] = array($time, $graphData['notkept']);
            $nodataSeries[] = array($time, $nodata);
        }


        $data['graphSeries']['labels'] = json_encode($labels);
        $data['graphSeries']['values'] = json_encode($values);
        $data['graphSeries']['keptseries'] = json_encode($keptSeries);
        $data['graphSeries']['repairedseries'] = json_encode($repairedSeries);
        $data['graphSeries']['notkeptseries'] = json_encode($notKeptSeries);
        $data['graphSeries']['nodataseries'] = json_encode($nodataSeries);


        // these are two extra parameters that has to be accessible in the bar chart graph
        if (is_array($count) && !empty($count))
        {
            $data['graphSeries']['count'] = json_encode($count);
        }

        if (is_array($start) && !empty($start))
        {
            $data['graphSeries']['start'] = json_encode($start);
        }
        return $data;
    }
}
?>

