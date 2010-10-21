<?php
require_once("../jCryption-1.1.php");
$jCryption = new jCryption();
$repo = 'https://svn.iu.hio.no/projects/Sudhir';
$working_dir = realpath('../policies');
$user=$_POST['user'];
$passwd=$_POST['passwd'];
session_start();
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
        'working'=>$working_dir
	     );
		session_unset();
        echo json_encode($data);
	}
elseif($op=='commit')
       {
        //session_start();
        $path=$working_dir.'/'.$_POST['file'];
        $comment=$_POST['comments'];
        $file_stat=svn_status($path);
        if($file_stat[0]['text_status']==2)
         {
         svn_add(realpath($path));
         }
        $cdetails=svn_commit($comment,array(realpath($path)));
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
	   $path=$working_dir.'/'.$_POST['file'];
       $filestat=svn_update(realpath($working_dir));
	   session_unset();
       echo $filestat;
      }

elseif($op='log')
     {
		$headrev=SVN_REVISION_HEAD;
		$limit=100;
		$logstable="<div class=\"tables\"><table><thead><tr><th>Revision</th><th>Author</th><th>Msg</th><th>date</th><th>path</th></tr></thead><tbody>";
		$logs=svn_log($repo,$headrev,0,$limit);
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
		echo $logstable;
     }

?>
