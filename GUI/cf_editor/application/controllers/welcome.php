<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Welcome extends Controller {

	function Welcome()
	{
		parent::Controller();
		$this->load->helper('url');	
	}
	
	function index()
	{
		//$this->template->write_view('content','mockup');
		//$this->template->write_view('content', 'editor');
		$this->load->view('cfeditor');
		//$this->template->render();
		//$this->load->view('welcome_message');
	}
}
?>
