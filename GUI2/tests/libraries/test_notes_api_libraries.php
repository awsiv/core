<?php

class test_notes_api_libraries extends CodeIgniterUnitTestCase {

    public function __construct() {
        parent::__construct();
    }

    public function setUp() {

    }

    public function tearDown() {

    }

    public function test_getValidJsonForNotes() {

        $comments = cfpr_query_note(NULL, NULL, NULL, NULL, NULL);
        $comments = utf8_encode($comments);
        $data = json_decode($comments, TRUE);
        $this->assertTrue(is_array($data));
    }

    

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */