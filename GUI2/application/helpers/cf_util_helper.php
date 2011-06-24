<?php

/**
 * for checking the ajax request
 * @return <type>
 */
function is_ajax() {
    return (isset($_SERVER['HTTP_X_REQUESTED_WITH']) &&
    ($_SERVER['HTTP_X_REQUESTED_WITH'] == 'XMLHttpRequest'));
}

/**
 *
 * @param <type> $message string  to be displayed
 * @return <type> html string
 */
function set_success_limiters($message) {
    return '<p class="success">' . $message . '</p>';
}

/**
 *
 * @param int $page as page number of current page
 * @param <type> $rp as rows per page
 * @param <type> $total as total number of rows
 * @param int $limit as limit of pages to be displayed
 * @return <type>
 */
function paging($page, $rp, $total, $limit) {
    $limit -= 1;

    $mid = floor($limit / 2);

    if ($total > $rp)
        $numpages = ceil($total / $rp);
    else
        $numpages = 1;

    if ($page > $numpages)
        $page = $numpages;

    $npage = $page;

    while (($npage - 1) > 0 && $npage > ($page - $mid) && ($npage > 0))
        $npage -= 1;

    $lastpage = $npage + $limit;

    if ($lastpage > $numpages) {
        $npage = $numpages - $limit + 1;
        if ($npage < 0)
            $npage = 1;
        $lastpage = $npage + $limit;
        if ($lastpage > $numpages)
            $lastpage = $numpages;
    }

    while (($lastpage - $npage) < $limit)
        $npage -= 1;

    if ($npage < 1)
        $npage = 1;

    //echo $npage; exit;

    $paging['first'] = 1;
    if ($page > 1)
        $paging['prev'] = $page - 1; else
        $paging['prev'] = 1;
    $paging['start'] = $npage;
    $paging['end'] = $lastpage;
    $paging['page'] = $page;
    if (($page + 1) < $numpages)
        $paging['next'] = $page + 1; else
        $paging['next'] = $numpages;
    $paging['last'] = $numpages;
    $paging['total'] = $total;
    $paging['iend'] = $page * $rp;
    $paging['istart'] = ($page * $rp) - $rp + 1;

    if (($page * $rp) > $total)
        $paging['iend'] = $total;

    return $paging;
}

/**
 * For using the array returned as a list  for autocompletion
 * @param <type> $report as array  and the column ie filter for which the autocompletion is to be done
 * @return json array
 * @author sudhir
 */
function autocomplete($report_data, $column) {
    $decoded_data = json_decode($report_data);
    $column_index = $decoded_data->meta->header->$column;
    $column = array();
    foreach ($decoded_data->data as $rows) {
        array_push($column, $rows[$column_index]);
    }
    $unique_elements = array_unique($column);
    if (is_array($unique_elements)) {  //necessary because the array_unique some times return associative array
        $val = array_values($unique_elements);
        return json_encode($val);
    } else {
        return json_encode($unique_elements);
    }
}

function host_only_table($data_array) {
    $table = "";
    $cells = array();

    if (count($data_array) > 0) {
        foreach ($data_array as $cols) {
            //$keys = array_keys($cols); if key valuesupplied in format{"key":"","id":""}
            array_push($cells, anchor('welcome/host/' . $cols['hostkey'], $cols['hostname'], 'class="imglabel"'));
            //use first key as param and second key as text as keys tend to change like key or hostkey, hostname or id etc
           // array_push($cells, anchor('welcome/host/' . $cols[1], $cols[0], 'class="imglabel"') . anchor('visual/vital/' . $cols[1], "View vitals", 'class="viewvitalslnk"'));
        }
    }
    if (count($cells) > 0) {
        $table.="<table><tr>";
        for ($i = 0; $i < count($cells); $i++) {
            if ($i % 6 == 0 && $i != 0) {
                $table.='</tr><tr>';
            }
            $table.='<td>' . $cells[$i] . '</td>';
        }
        $table.="</tr></table>";
    } else {
        $table.="<table><tr><td>No Host Found !!!</td></tr></table>";
    }
    return $table;
}

function getonlineusernames()
    {
       $onlineuser=array();
        $CI = get_instance();
        $CI->load->library(array('mongo_db','ion_auth'));
        $result=$CI->mongo_db->get('onlineusers');
         foreach ($result as  $docs) {
            $obj=(object)$docs;
            $user=$CI->ion_auth->get_user($obj->user_id);
            array_push($onlineuser, $user->username);
           //var_dump($user);
         }
       return $onlineuser;
    }

/**
 * Formats a timestamp and then gives it a color code
 * amber when time difference is greater than 6 hours
 * red when older than 7 days
 * @param <type> $timestamp
 * @return <string> datetime with wrapped in span with colorclass
 */
function getDateStatus($timestamp,$noColor=false) {
    
    $timestamp = intval($timestamp,10);
    $colorClass = '';
    $now = time();
    // 6 hours
    if ($now - $timestamp > 6*3600) {
        $colorClass = 'amber';
    }
    // 7 days
    if ($now - $timestamp > 7*24*3600) {
        $colorClass = 'red';
    }
    if ($noColor) $colorClass = '';
    $formattedDate = date('D F d h:i:s Y',$timestamp);
    return "<span class=$colorClass>$formattedDate</span>";
    
}

/**
 *
 * @param <type> $url
 * @param <type> $_data
 *
 */
function notifier($url,$_data)
{
    //$_data=array("name"=>"sudhir","login"=>true);
        $data = array();
       while(list($n,$v) = each($_data)){
        $data[] = urlencode($n)."=".urlencode($v);
       }
      $data = implode('&', $data);
       $ch = curl_init();
       
        // set URL and other appropriate options
        curl_setopt($ch, CURLOPT_URL,$url);
        curl_setopt($ch, CURLOPT_HEADER, false);
        curl_setopt ($ch, CURLOPT_POST, true);
        curl_setopt ($ch, CURLOPT_POSTFIELDS, $data);
        // grab URL and pass it to the browser
        curl_exec($ch);

        // close cURL resource, and free up system resources
        curl_close($ch);
}

?>
