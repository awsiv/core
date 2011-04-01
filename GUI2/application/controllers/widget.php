<?php
class Widget extends CI_Controller
{

	function Widget()
	 {
		 parent::__construct();

	 }


        function hostfinder()
        {
            $this->data['hostlist']=json_decode(cfpr_select_hosts("none", ".*", 100));
            $this->load->view('widgets/hostfinder',$this->data);
        }
}

?>
