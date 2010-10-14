<?php
require_once("../jCryption-1.1.php");
$jCryption = new jCryption();
$repo = 'https://svn.iu.hio.no/projects/Sudhir';
$working_dir = realpath('../policies');
$user=$_POST['user'];
$passwd=$_POST['passwd'];
$var = $jCryption->decrypt($_POST['passwd'], $_SESSION["d"]["int"], $_SESSION["n"]["int"]);
svn_auth_set_parameter(SVN_AUTH_PARAM_DEFAULT_USERNAME, $user);
svn_auth_set_parameter(SVN_AUTH_PARAM_DEFAULT_PASSWORD, $var);

svn_auth_set_parameter(SVN_AUTH_PARAM_DONT_STORE_PASSWORDS, true);
svn_auth_set_parameter(PHP_SVN_AUTH_PARAM_IGNORE_SSL_VERIFY_ERRORS,true);
svn_auth_set_parameter(SVN_AUTH_PARAM_NO_AUTH_CACHE,true);

$op=$_POST['op'];
if($op=='checkout')
	{
        $status=svn_checkout($repo, $working_dir);
        $data=array(
		'status'=>$status,
                'working'=>$working_dir
	     );
        echo json_encode($data);
	}
elseif($op=='commit')
       {
        session_start();
        $path=$working_dir.'/'.$_POST['file'];
        //$user=$_POST['user'];
        //$passwd=$_POST['passwd'];
        $comment=$_POST['comment'];
        //$var = $jCryption->decrypt($_POST['passwd'], $_SESSION["d"]["int"], $_SESSION["n"]["int"]);
        //svn_auth_set_parameter(SVN_AUTH_PARAM_DEFAULT_USERNAME, $user);
        //svn_auth_set_parameter(SVN_AUTH_PARAM_DEFAULT_PASSWORD, $var);
       $file_stat=svn_status($path);
        if($file_stat[0]['text_status']==2)
         {
         svn_add(realpath($path));
         }
        $cdetails=svn_commit($comment,array(realpath($path)));
        echo json_encode ($cdetails);
       }
elseif($op=='update')
      {
       $dirstat=svn_update($working_dir);
       echo $dirstat;
      }
  
?>
