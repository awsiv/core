<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//this file contains the functions that generates paths to different portion of the application

function get_cssdir()
	{
	$CI=&get_instance();
        if($CI->session->userdata('theme'))
         {
	  return base_url().'themes/'.$CI->session->userdata('theme').'/'.$CI->config->item('css_dir');
         }
         else {

         return base_url().$CI->config->item('default').$CI->config->item('css_dir');
          }
	}

function get_scriptdir()
	{
	 $CI=&get_instance();
	 return base_url().$CI->config->item('scripts_dir');
	}

function get_imagedir()
	{
	 $CI=&get_instance();
	 return base_url().$CI->config->item('image_dir');
	}

function get_hubdir()
	{
	 $CI=&get_instance();
	 //return base_url().
	 return $CI->config->item('hub_dir');
	}

function get_graphdir()
	{
	$CI=&get_instance();
	 //return base_url().$CI->config->item('graph_dir');
	 return $CI->config->item('graph_dir');
	}

function get_policiesdir()
	{
	 $CI=&get_instance();
	 return $CI->config->item('policies_dir');
	}
        
function is_ajax() {
    return (isset($_SERVER['HTTP_X_REQUESTED_WITH']) &&
    ($_SERVER['HTTP_X_REQUESTED_WITH'] == 'XMLHttpRequest'));
}

function set_success_limiters($message)
{
  return '<p class="success">'.$message.'</p>';
}

function paging($page,$rp,$total,$limit)
{
        $limit -= 1;

        $mid = floor($limit/2);

        if ($total>$rp)
            $numpages = ceil($total/$rp);
        else
            $numpages = 1;

        if ($page>$numpages) $page = $numpages;

            $npage = $page;

        while (($npage-1)>0&&$npage>($page-$mid)&&($npage>0))
            $npage -= 1;

        $lastpage = $npage + $limit;

        if ($lastpage>$numpages)
            {
            $npage = $numpages - $limit + 1;
            if ($npage<0) $npage = 1;
            $lastpage = $npage + $limit;
            if ($lastpage>$numpages) $lastpage = $numpages;
            }

        while (($lastpage-$npage)<$limit) $npage -= 1;

        if ($npage<1) $npage = 1;

        //echo $npage; exit;

        $paging['first'] = 1;
        if ($page>1) $paging['prev'] = $page - 1; else $paging['prev'] = 1;
        $paging['start'] = $npage;
        $paging['end'] = $lastpage;
        $paging['page'] = $page;
        if (($page+1)<$numpages) $paging['next'] = $page + 1; else $paging['next'] = $numpages;
        $paging['last'] = $numpages;
        $paging['total'] = $total;
        $paging['iend'] = $page * $rp;
        $paging['istart'] = ($page * $rp) - $rp + 1;

        if (($page * $rp)>$total) $paging['iend'] = $total;

        return $paging;
}
?>