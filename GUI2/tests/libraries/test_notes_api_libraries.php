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

        $comments = cfpr_query_note('', '', '', -1, -1, 0, 0);
        $comments = str_replace(array("\r\n", "\n", "\r"), '<br />', utf8_encode(trim($comments)));
        $this->dump($comments);
        $data = json_decode($comments, TRUE);
        $this->assertTrue(is_array($data));
    }

    public function test_getTotalNotes() {

        $comments = cfpr_query_note('', '', '', -1, -1, 0, 0);
        $comments = str_replace(array("\r\n", "\n", "\r"), '<br />', utf8_encode(trim($comments)));

        $data = json_decode($comments, TRUE);
        //$this->dump($data);
        $totalcount = $data['meta']['count'];
        $totaldata = count($data['data']);
        $this->assertTrue(is_array($data) && ($totalcount == $totaldata), "Data returned must be equal count Total Count :: $totalcount , Returned DataCount :: $totaldata");
    }

    /**
     * must return only 1 record
     */
    public function test_getValidJsonForNotesPagination() {

        $comments = cfpr_query_note('', '', '', -1, -1, 1, 1);
        $comments = str_replace(array("\r\n", "\n", "\r"), '<br />', utf8_encode(trim($comments)));

        $data = json_decode($comments, TRUE);
        $count = count($data['data']);
        $totalcount = $data['meta']['count'];
        //$this->dump($data);
        if ($totalcount > 0) {
            $this->assertTrue(is_array($data) && ($count == 1), "Should be 1 ,  coming $count");
        }
    }

}

/* End of file test_123_bug.php */
/* Location: ./tests/bugs/test_123_bug.php */