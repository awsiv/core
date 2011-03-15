<?php
class Notes extends Cf_Controller{

	function __construct()
		{
			parent::__construct();
		$this->load->library('form_validation');
                $this->form_validation->set_error_delimiters('<span class="errorlist">', '</span>');
		}

        function index()
        {
            
        }

        function add($hostkey)
        {
            $this->form_validation->set_rules('comment_text', 'comment_text', 'required|valid_email');
		if ($this->form_validation->run() == true)
		{ //check to see if the user is logging in
                     //$username=$this->input->post('user_name');
                     $note=$this->input->post('comment_text');
                     $hostkey=$this->input->post('host_key');
                     $noteid = cfpr_get_host_noteid($hostkey);
                     cfpr_add_note($commentid,$username,time(),$comment_text);
                     $this->data['message']="Comments Sucessfully added";
                    
                }
                else
                {
                    $this->data['message'] = (validation_errors()) ? validation_errors() : $this->session->flashdata('message');
                    $this->data['lbl_email']="Email";
                    $this->data['hostkey']=$hostkey;
		    $this->data['note'] = array('name' => 'comment',
				'id' => 'comment',
				'type' => 'text',
				'value' => $this->form_validation->set_value('text'),
                               );
                    $this->load->view('notes/add_notes',$this->data);
                }
        }

        function show_notes($hostkey)
        {
         $noteid = cfpr_get_host_noteid($hostkey);
          $comments=cfpr_query_note($hostkey,$noteid,'',-1,-1);
         echo $comments;
        }
}

?>
