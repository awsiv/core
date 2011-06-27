<?php

class test_notes_model extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {
        $this->_ci->load->model('note_model');
       
    }

    public function tearDown() {
        
    }

    public function test_getAllNotes() {
        $notes = $this->_ci->note_model->getAllNotes();
        $this->assertTrue(is_array($notes), "Should Return a valid array");
    }
    
    public function test_testNoteType() {
        $notes = $this->_ci->note_model->getAllNotes();
        if (!empty($notes)) {
            // should be one notes instance 
            $this->assertIsA($notes[0], 'CF_Note');
        }
    }
    
    

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */