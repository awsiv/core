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

/**
 * For drawing the table of hosts from the array of data supplied
 * @param <type> $data_array
 * @return string html
 * @author sudhir
 */
function host_only_table($data_array) {
    $table = "";
    $cells = array();

    if (count($data_array) > 0) {
        foreach ($data_array as $cols) {
            //$keys = array_keys($cols); if key valuesupplied in format{"key":"","id":""}
            //array_push($cells, anchor('welcome/host/' . $cols[$keys[0]], $cols['hostname'], 'class="imglabel"'));
            //use first key as param and second key as text as keys tend to change like key or hostkey, hostname or id etc
            array_push($cells, anchor('welcome/host/' . $cols[1], $cols[0], 'class="imglabel"') . anchor('visual/vital/' . $cols[1], "View vitals", 'class="viewvitalslnk"'));
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

function object_to_array($object) {
    if (!is_object($object) && !is_array($object)) {
        return $object;
    }
    if (is_object($object)) {
        $object = get_object_vars($object);
    }
    return array_map('object_to_array', $object);
}

function create_json_node_for_report_control() {
    $CI = get_instance();
    $CI->load->library('reportnode');
    $node = array(
        'id' => 'node0',
        'name' => 'report name',
        'data' => array(),
        'children' => array()
    );

    $reportcategories = array("compliance" => array(), "profile" => array(), "software" => array(), "promise logs" => array(), "file logs" => array());
    $reports = json_decode(cfpr_select_reports(".*", 100));

    foreach ($reports as $report) {
        switch ($report) {
            case "Bundle profile":
            case "Business value report":
            case "Compliance by promise":
                array_push($reportcategories['compliance'], $report);
                break;

            case "Variables":
            case "Class profile":
            case "Last saw hosts":
            case "Performance":
                array_push($reportcategories['profile'], $report);
                break;

            case "Software installed":
            case "Patches available":
            case "Patch status":
                array_push($reportcategories['software'], $report);

                break;
            case "Promises repaired log":
            case "Promises repaired summary":
            case "Promises not kept log":
            case "Promises not kept summary":
            case "Compliance summary":
                array_push($reportcategories['promise logs'], $report);
                break;

            case "File change log":
            case "File change diffs":
            case "Setuid/gid root programs":
                array_push($reportcategories['file logs'], $report);
                break;
        }
    }

    //print_r($reportcategories);
    $node['id'] = 'reports';
    $node['name'] = 'reports';
    $node['data'] = array('$type' => 'none');
    $rootnode = $CI->reportnode->createnode($node);
    $i = 0;
    foreach ($reportcategories as $category => $reports) {
        $node['name'] = $category;
        $node['id'] = $category;

        $node['data'] = array(
            'description' => 'bla bla',
            'color' => '#fefefe',
            'hover-in' => '#dd3333',
        );
        $parent = $CI->reportnode->createnode($node);
        foreach ($reports as $report) {

            $node['id'] = 'node' . $i;
            $node['name'] = $report;
            $child = $CI->reportnode->createnode($node);
            array_push($parent->children, $child);
            $i++;
        }
        array_push($rootnode->children, $parent);
    }
    // var_dump($rootnode);
    $array = object_to_array($rootnode);
    //print_r($array);
    return json_encode($array);
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

?>
