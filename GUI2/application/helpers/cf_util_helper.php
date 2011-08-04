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
            if ($i % 3 == 0 && $i != 0) {
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

function getonlineusernames() {
    $onlineuser = array();
    $CI = get_instance();
    $CI->load->library(array('mongo_db', 'ion_auth'));
    $result = $CI->mongo_db->get('onlineusers');
    foreach ($result as $docs) {
        $obj = (object) $docs;
        //$user=$CI->ion_auth->get_user($obj->user_id);
        // array_push($onlineuser, $user->username);
        array_push($onlineuser, $obj->user_id);
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
function getDateStatus($timestamp, $noColor=false) {

    $timestamp = intval($timestamp, 10);
    $colorClass = '';
    $now = time();
    // 6 hours
    if ($now - $timestamp > 6 * 3600) {
        $colorClass = 'amber';
    }
    // 7 days
    if ($now - $timestamp > 7 * 24 * 3600) {
        $colorClass = 'red';
    }
    if ($noColor)
        $colorClass = '';
    $formattedDate = date('c', $timestamp);
    return '<span class="localtime ' . $colorClass . '">' . $formattedDate . '</span>';
}

/**
 *
 * @param <type> $url
 * @param <type> $_data
 *
 */
function notifier($url, $_data) {
    //$_data=array("name"=>"sudhir","login"=>true);
    $data = array();
    while (list($n, $v) = each($_data)) {
        $data[] = urlencode($n) . "=" . urlencode($v);
    }
    $data = implode('&', $data);
    $ch = curl_init();

    // set URL and other appropriate options
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_HEADER, false);
    curl_setopt($ch, CURLOPT_POST, true);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
    // grab URL and pass it to the browser
    curl_exec($ch);

    // close cURL resource, and free up system resources
    curl_close($ch);
}

//checking the format of json data returned from the core before echoing back to user
function sanitycheckjson($data, $noecho=false) {
    //php 5.1.6 only have json_encode and json_decode and red hat only have this 5.1.6 ats stable package so need to support it as well
    $data_check = json_decode(utf8_encode($data), true);
    $CI = &get_instance();
    if (function_exists("json_last_error")) {
        if (json_last_error() == 0) {
            if (!empty($data_check)) {
                if ($noecho) {
                    return $data_check;
                }
                echo $data;
            } else {
                $CI->output->set_status_header('400', 'No data Found');
            }
        } else {
            $CI->output->set_status_header('500', 'Valid Json Data cannot be generated from Promises');
        }
    } else {
        if ($data_check == NULL) {
            $CI->output->set_status_header('500', 'Valid Json Data cannot be generated from Promises');
        } else {
            if (is_array($data_check) && !empty($data_check)) {
                if ($noecho) {
                    return $data_check;
                }
                echo $data;
            } else {
                $CI->output->set_status_header('400', 'No data Found');
            }
        }
    }
}

function is_execptional_body($body) {
    $list = array('usebundle', 'edit_line');
    if (array_search($body, $list) !== FALSE) {
        return True;
    }
    return false;
}

function array_msort($array, $cols, $natsort=false) {
    $colarr = array();
    foreach ($cols as $col => $order) {
        $colarr[$col] = array();
        foreach ($array as $k => $row) {
            $colarr[$col]['_' . $k] = strtolower($row[$col]);
        }
    }
    $eval = 'array_multisort(';
    foreach ($cols as $col => $order) {
        $eval .= '$colarr[\'' . $col . '\'],' . $order . ',';
    }
    $eval = substr($eval, 0, -1) . ');';
    eval($eval);
    if ($natsort) {
        foreach ($cols as $col => $order) {
            natsort($colarr[$col]);
        }
    }
    $ret = array();
    foreach ($colarr as $col => $arr) {
        foreach ($arr as $k => $v) {
            $k = substr($k, 1);
            if (!isset($ret[$k]))
                $ret[$k] = $array[$k];
            $ret[$k][$col] = $array[$k][$col];
        }
    }
    return $ret;
}

/**
 * Lang
 *
 * Fetches a language variable and optionally outputs a form label for tooltips
 *
 * @access	public
 * @param	string	the language line
 * @param	string	the id of the form element
 * @return	string
 */
function tooltip($line, $id = '') {
    $CI = & get_instance();
    // if tooltip is disabled in preferences return blank
    if (!$CI->setting_lib->get_tooltips_status())
        return '';

    $line = $CI->lang->line($line);

    if ($id != '') {
        $line = '<label for="' . $id . '">' . $line . "</label>";
    }

    return $line;
}

?>
