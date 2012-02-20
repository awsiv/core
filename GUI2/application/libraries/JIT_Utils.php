<?php

class JIT_Utils {
    
    public function meters_to_jit($rawData) {
        $convertedData = json_decode($rawData, true);

        $values = array();
        $labels = array('kept', 'repaired', 'not kept', 'no data');
        $count = array(); // for keeping tracks of each count
        $start = array(); // the timestamp passed of each node starttime.
        foreach ($convertedData as $key => $graphData) {
            $nodata = (isset($graphData['nodata'])) ? $graphData['nodata'] : 0;
            $values[] = array('label' => $graphData['title'],
                'values' => array($graphData['kept'], $graphData['repaired'], $graphData['notkept'], $nodata));

            // track the count parameter
            // because we cannot directly pass the custom data in barChart of infovis
            if (isset($graphData['count'])) {
                $count[$graphData['title']] = $graphData['count'];
            }
            if (isset($graphData['start'])) {
                $start[$graphData['title']] = $graphData['start'];
            }
        }
        
        $data = array();
        $data['labels'] = $labels; //json_encode($labels);
        $data['values'] = $values; //json_encode($values);

        // these are two extra parameters that has to be accessible in the bar chart graph
        if (is_array($count) && !empty($count)) {
            $data['count'] = json_encode($count);
        }

        if (is_array($start) && !empty($start)) {
            $data['start'] = json_encode($start);
        }
        return $data;
    }
}

?>
