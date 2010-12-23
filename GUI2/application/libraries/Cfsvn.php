<? if (!defined('BASEPATH')) exit('No direct script access allowed');
//for this class to work we have to install the php svn module support in php engine 
class Cfsvn
{
  private $working_dir;
  private $repository;
  private $username;
  private $password;
  
  
  function Cfsvn($params)
  {
  $this->username=$params['username'];
  $this->password=$params['password'];
  $this->working_dir=isset($params['workingdir'])?$params['workingdir']:NULL;
  svn_auth_set_parameter(SVN_AUTH_PARAM_DEFAULT_USERNAME,$this->username);
  svn_auth_set_parameter(SVN_AUTH_PARAM_DEFAULT_PASSWORD, $this->password);
  svn_auth_set_parameter(SVN_AUTH_PARAM_DONT_STORE_PASSWORDS, true);
  svn_auth_set_parameter(PHP_SVN_AUTH_PARAM_IGNORE_SSL_VERIFY_ERRORS,true);
  svn_auth_set_parameter(SVN_AUTH_PARAM_NO_AUTH_CACHE,true);
	if(!file_exists($this->working_dir)&&isset($this->working_dir))
	  {
		if(!mkdir($this->working_dir,0700))
		{
		  echo "cannot create the working directory";
		  return;
		}
	  }
	$this->repository=isset($params['repository'])?$params['repository']:NULL;
  }
  
  function cfsvn_checkout()
  {
        $status=svn_checkout($this->repository, $this->working_dir);
        $data=array(
		'status'=>$status,
        'working'=>$this->working_dir,
		'session'=>session_id()
	     );
        return $data;
  }
  
  ///function for commiting a particular file or a whole directory depends upon the working dir parameter
  function cfsvn_commit($comment)
  {
      $file_stat=svn_status($this->working_dir);
		foreach ($file_stat as $eachfile)
		{
		  if($eachfile['text_status']==2)
		  {
			svn_add($eachfile['path']);  
		  }
		}
      $cdetails=svn_commit($comment,array($this->working_dir));
	  return $cdetails;
  }
  
  function cfsvn_update()
  {
   $filestat=svn_update($this->working_dir);
   return $filestat;
  }
  
  function cfsvn_log($limit)
  {
   $logs=svn_log($this->repository,SVN_REVISION_HEAD,0,$limit);
   return $logs;
  }

}