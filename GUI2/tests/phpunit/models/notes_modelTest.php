<?php

class test_notes_model extends CI_BaseTestCase
{

    private $ci_obj;

    public function setUp()
    {
        parent::setUp();
        // Instantiate a new loader
        $this->load = new Mock_Core_Loader();

        // mock up a ci instance
        $this->ci_obj = new StdClass;

        // Fix get_instance()
        $this->ci_instance($this->ci_obj);

          $this->load->model('note_model');
    }

    public function test_model_exist()
    {
        $this->ci_set_core_class('model', 'CI_Model');


        $this->assertNull($this->load->model('note_model'));

        // Was the model class instantiated.
        $this->assertTrue(class_exists('note_model'));
    }

    public function test_getAllNotes()
    {
        $filter = array('loggedUser'=>'admin','userId'=>'admin');
        $notes = $this->ci_obj->note_model->getAllNotes($filter);
        $this->assertTrue(is_array($notes), "Should Return a valid array");
    }

    public function test_testNoteType()
    {
        $filter = array('loggedUser'=>'admin','userId'=>'admin');
        $notes = $this->ci_obj->note_model->getAllNotes($filter);
        if (!empty($notes))
        {
            // should be one notes instance
            $this->assertInstanceOf('CF_Note',$notes[0]);
        }
    }




}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */