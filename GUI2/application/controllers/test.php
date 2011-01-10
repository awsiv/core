<?php

class Test extends Controller
{
	
	function Test()
	 {
		 parent::Controller();
     
	 }
	
	function mongodbtest()
	 {
	    $this->load->library('mongo_db');
		$output=$this->mongo_db->insert('app_logs',array(

		// Server Info
		'server_name'	=> $_SERVER['SERVER_NAME'],
		'server_ip'		=> (isset($_SERVER['SERVER_ADDR'])) ? $_SERVER['SERVER_ADDR'] : '0.0.0.0',
		'domain'		=> (!empty($_SERVER['HTTP_HOST'])) ? $_SERVER['HTTP_HOST'] : '',

		//User Info
		'user_agent'	=> (!empty($_SERVER['HTTP_USER_AGENT'])) ? $_SERVER['HTTP_USER_AGENT'] : '',
		'ip_address'	=> (!empty($_SERVER['REMOTE_ADDR'])) ? $_SERVER['REMOTE_ADDR'] : '',
		'uri'			=> (!empty($_SERVER['REQUEST_URI'])) ? $_SERVER['REQUEST_URI'] : '',
		'query_string'	=> (!empty($_SERVER['QUERY_STRING'])) ? $_SERVER['QUERY_STRING'] : '',

		'timestamp'		=> date('Y-m-d H:i:s'),
		'message'		=> "testing",
		'level'			=> "0",
		));
		echo $output;
	 } 
	 
	 function mysqltest()
	 {
	   $this->load->database();
	   //$dbhandle = mysql_connect("localhost", "root", "") 
           // or die("Unable to connect to MySQL");
           echo "Connected to MySQL<br>";

	 }

}