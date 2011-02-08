<?php
class Notes extends Cf_Controller{

	function __construct()
		{
			parent::__construct();
			
		}

        function index()
        {
            
        }

        function add()
        {
            $username=$this->input->post('user_name');
            $note=$this->input->post('comment_text');
            $hostkey=$this->input->post('host_key');
            $commentid = cfpr_get_host_commentid($hostkey);
            cfpr_comment_add($hostkey,"",$commentid,1,"$hostname,$ipaddr",$username,time(),$comment_text);
        }

        function show_notes($hostkey)
        {
         $commentid = cfpr_get_host_commentid($hostkey);
	 $comments=cfpr_comment_query('',$commentid,'',-1,-1);
         echo $comments;
        }
}

?>
