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

    // if row per page == 0 - set it to 1
    if (empty($rp)) {
        $rp = 1;
    }
    
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
    
    $paging['nextSet']=$lastpage+1;
    $paging['prevSet']=$npage-1;
    
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


function getonlineusernames() {
    $onlineuser = array();
    $CI = &get_instance();
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
 * @param int $timestamp 
 * @param bool $noColor HTML with no color class
 * @param bool $onlyDate no html output
 * @return <string> datetime with wrapped in span with colorclass
 * 
 */
function getDateStatus($timestamp, $noColor = false, $onlyDate = false) {

    $timestamp = intval($timestamp, 10);
    // check for 0 val 
    if ($timestamp === 0) {
        if ($onlyDate)
            return 0;
        return '<span class="red">(never)</span>';
    }

    $colorClass = '';
    $now = time();
    // 6 hours
    if ($now - $timestamp > 6 * 3600) {
        $colorClass = 'amber';
        $iconClass = 'yellowDateStatus';
    }
    // 7 days
    if ($now - $timestamp > 7 * 24 * 3600) {
        $colorClass = 'red';
        $iconClass = 'redDateStatus';
    }
    if ($noColor) {
        $colorClass = '';
    }


   

    /**
     * return the date with respect to users time zone set on the session 
     *  
     */
    $script_tz = date_default_timezone_get(); // backup the default timezone
    $CI = & get_instance();
    $tz_string = $CI->session->userdata('user_timezone');
    if (!date_default_timezone_set($tz_string)) {
         date_default_timezone_set($script_tz); // if not invalid timezone identifier set it.
    }
    $formattedDate = date('M jS Y H:i', $timestamp);
    // add time zone info
    $timeZoneinfo = date ('P',$timestamp);
    $formattedDate = $formattedDate.sprintf(" (GMT %s)",$timeZoneinfo);
    date_default_timezone_set($script_tz); // reset back to default timezone
    if ($onlyDate) {
        return $formattedDate;
    }

    return '<span class="localtime ' . $colorClass . '">' . $formattedDate . '</span>';
}

/**
 * Format seconds to day , hour min and seconds
 * @param type $s no of seconds
 * @return string 
 */
function formatSeconds($s) {
    $d = intval($s / 86400);
    $s -= $d * 86400;
    if ($s <= 0)
        return '0s';
    $str = '';
    $h = intval($s / 3600);
    $s -= $h * 3600;

    $m = intval($s / 60);
    $s -= $m * 60;

    if ($d)
        $str = $d . 'd ';
    if ($h)
        $str .= $h . 'h ';
    if ($m)
        $str .= $m . 'm ';
    if ($s)
        $str .= $s . 's';

    return $str;
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
           if ($noecho) return $data_check;
            if (!empty($data_check)) {
                echo $data;
            } else {
                echo "";
            }
        } else {
           $CI->output->set_status_header('500', 'Valid Json Data cannot be generated');
           if ($noecho) return array();
        }
    } else {
        if ($data_check == NULL) {
            $CI->output->set_status_header('500', 'Valid Json Data cannot be generated');
            if ($noecho) return array();
        } else {
            if ($noecho) return $data_check;
            if (is_array($data_check) && !empty($data_check)) {
                echo $data;
            } else {
                echo "";
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
function tooltip($line, $id = '',$forinput=false) {
    $CI = & get_instance();
    // if tooltip is disabled in preferences return blank
    if (!$CI->setting_lib->get_tooltips_status())
        return '';

    $line = $CI->lang->line($line);

    if ($id != '') {
            $line = '<label for="' . $id . '">' . $line . "</label>";
    }elseif($forinput){
            $line ='<span class="hint">'. $line.'</span>';
    }
   
    return $line;
}

/**
 * time conversion 
 *
 * converts time difference into readable format (like: "12d 05h 10m") 
 *
 * @access	public
 * @param	string   delta timestamp	
 * @return	string
 */
function time_diff_conv($s) {
    $string = "";
    $t = array( //suffixes
        'days' => 86400,
        'hours' => 3600,
        'minutes' => 60,
    );
    $s = abs($s);
    foreach($t as $key => &$val) {
        $$key = floor($s/$val);
        $s -= ($$key*$val);
        $string .= ($$key==0) ? '' : $$key . " $key ";
    }

    return $string ;
}

/**
 *
 * @param type $exception
 * @return string 
 */
function generate_errormessage($exception){
    $message="";
     $CI = & get_instance();
    
    switch (ENVIRONMENT)
	{
		case 'development':
			$message= "<div style=\"border:1px solid #990000;padding-left:20px;margin:0 0 10px 0;\">
                        <h1>Error Occured: ".$exception->getMessage()."</h1><pre>" . $exception->getTraceAsString() . "<pre></div>";
                        log_message('error', $exception->getMessage().'Trace: '. $exception->getTraceAsString());
		break;
	
		case 'testing':
                       $message= "<h1>".$exception->getMessage()."</h1><pre>" . $exception->getTraceAsString() . "<pre>";
                        log_message('error', $exception->getMessage().'Trace: '. $exception->getTraceAsString());
                        
		case 'production':
			$message="<div class=\"error\">".$CI->lang->line('cf_mod_expection_error')."</div>"; 
                        log_message('error', $exception->getMessage().'File: '. $exception->getFile()." Line: ".$exception->getLine());
		break;

		default:
		       log_message('error', $exception->getMessage().'Trace: '. $exception->getTraceAsString());
	}
    return $message;
}


/**
* Error Handler with custom template
*
* This function lets us invoke the exception class and
* display errors using the standard error template located
* in application/errors/errors.php
* This function will send the error page directly to the
* browser and exit.
*
* @access	public
* @return	void
*/
	function show_error_custom($message, $status_code = 500, $heading = 'An Error Was Encountered', $template = 'error_plain')
	{
		$_error =& load_class('Exceptions', 'core');
		echo $_error->show_error($heading, $message, $template, $status_code);
		exit;
	}

        
    /**
     * Diff arrays 
     * @param array $aArray1
     * @param array $aArray2
     * @return array 
     */    
        
    function arrayRecursiveDiff($aArray1, $aArray2) {
    $aReturn = array();

    foreach ($aArray1 as $mKey => $mValue) {
        if (array_key_exists($mKey, $aArray2)) {
            if (is_array($mValue)) {
                $aRecursiveDiff = arrayRecursiveDiff($mValue, $aArray2[$mKey]);
                if (count($aRecursiveDiff)) {
                    $aReturn[$mKey] = $aRecursiveDiff;
                }
            } else {
                if ($mValue != $aArray2[$mKey]) {
                    $aReturn[$mKey] = $mValue;
                }
            }
        } else {
            $aReturn[$mKey] = $mValue;
        }
    }
    return $aReturn;
}

 function get_parameters_without_paging($array)
    {
        $temp = array();
        foreach ((array) $array as $key => $val)
        {
            if ($key <> "page" && $key <> "rows" && trim($val))
            {
                $temp[] = $key;
                $temp[] = urlencode($val);
            }
        }
        return implode('/', $temp);
    }

    function getCuttedText($text, $maxwords = 30, $maxchar = 300) {
        $sep = "'/ /'";
        $words = preg_split($sep, $text);

        $char = iconv_strlen($text, 'utf-8');
        $bigger = false;

        if (count($words) > $maxwords) {
            $text = join($sep, array_slice($words, 0, $maxwords));
            $bigger = true;
        }

        if ($char > $maxchar) {
            $text = iconv_substr($text, 0, $maxchar, 'utf-8');
            $bigger = true;
        }

        if ($bigger == true)
            $text .= '...';

        return $text;
    }
    

?>
