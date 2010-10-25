<?php
require_once("../jCryption-1.1.php");
$jCryption = new jCryption();
$repo=$_POST['repo'];
$working_dir = realpath('../policies');
session_start();
if(!file_exists($working_dir.'/'.session_id()))
  {
	if(mkdir($working_dir.'/'.session_id(),0700))
	{
	  $working_dir=$working_dir.'/'.session_id();
	}
  }
 else
   {
	$working_dir=$working_dir.'/'.session_id();   
   }
$user=$_POST['user'];
$passwd=$_POST['passwd'];
$var = $jCryption->decrypt($passwd, $_SESSION["d"]["int"], $_SESSION["n"]["int"]);
svn_auth_set_parameter(SVN_AUTH_PARAM_DEFAULT_USERNAME, $user);
svn_auth_set_parameter(SVN_AUTH_PARAM_DEFAULT_PASSWORD, $var);

svn_auth_set_parameter(SVN_AUTH_PARAM_DONT_STORE_PASSWORDS, true);
svn_auth_set_parameter(PHP_SVN_AUTH_PARAM_IGNORE_SSL_VERIFY_ERRORS,true);
svn_auth_set_parameter(SVN_AUTH_PARAM_NO_AUTH_CACHE,true);

$op=$_POST['op'];
 /*$status=svn_checkout($repo, $working_dir);
 $data=array(
		'op'=>$op,
        'user'=>svn_auth_get_parameter(SVN_AUTH_PARAM_DEFAULT_USERNAME) ,
		'passwd'=>svn_auth_get_parameter(SVN_AUTH_PARAM_DEFAULT_PASSWORD),
		'status'=>$status,
		'working'=>$working_dir
	     );
 echo json_encode($data);*/

if($op=='checkout')
	{
        $status=svn_checkout($repo, $working_dir);
        $data=array(
		'status'=>$status,
        'working'=>$working_dir,
		'session'=>session_id()
	     );
		session_unset();
        echo json_encode($data);
	}
elseif($op=='commit')
       {
		 if($_POST['file']!="")
		 {
         $working_dir=$working_dir.'/'.$_POST['file'];
		 }
        $comment=$_POST['comments'];
        $file_stat=svn_status($working_dir);
		foreach ($file_stat as $eachfile)
		{
		  if($eachfile['text_status']==2)
		  {
			svn_add($eachfile['path']);  
		  }
		}
        /*if($file_stat[0]['text_status']==2)
         {
         svn_add($working_dir);
         }*/
        $cdetails=svn_commit($comment,array($working_dir));
		session_unset();
        echo json_encode ($cdetails);
		/*$data=array(
	    'user'=>svn_auth_get_parameter(SVN_AUTH_PARAM_DEFAULT_USERNAME) ,
		'passwd'=>svn_auth_get_parameter(SVN_AUTH_PARAM_DEFAULT_PASSWORD),
        'working'=>$path,
		'comment'=>$comment
	     );
        echo json_encode($data);*/
       }
elseif($op=='update')
      {
	   //$path=$working_dir.'/'.$_POST['file'];
       $filestat=svn_update($working_dir);
	   session_unset();
       echo $filestat;
      }

elseif($op='log')
     {
		$headrev=SVN_REVISION_HEAD;
		$limit=50;
		$logstable="<div class=\"tables\"><table><thead><th>Revision</th><th>Author</th><th>Msg</th><th>date</th><th>path</th></thead><tbody>";
		$logs=svn_log($repo,$headrev,0,$limit);
		if(count($logs) >0)
		 {
			foreach ($logs as $rows)
			{
				$logstable.="<tr>";
				foreach ($rows as $column=>$value)
				{
							 if($column=="paths")
							   {
								   $innertable="<table>";
								   foreach ($value as $innerrows)
									{
									   $innertable.="<tr>";
									   foreach ($innerrows as $innercol=>$colval)
									   {
										 $innertable.="<td>$colval</td>";  
									   }
									   $innertable.="</tr>";
									}
									$innertable.="</table>";
									$logstable.="<td>$innertable</td>";
							   }
							   else
							   {
								   $logstable.="<td>$value</td>";
							   }
				 
				}
				$logstable.="</tr>";
			}
		$logstable.="</tbody></table><div>";
		$data=array(
			'status'=>'success',
			'table'=>$logstable
			 );
		echo json_encode ($data);
		 }
	  else
		 {
			$data=array(
			'status'=>'Error',
			 );
		echo json_encode ($data);
		 }	 
     }

?>
