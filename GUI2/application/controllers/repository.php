<?php

class Repository extends CI_Controller {

    function __construct() {
        parent::__construct();
        $this->load->model('repository_model');
    }

    function insertRepository() {
        $info = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2', 'username' => 'something', 'password' => 'password');
        $return = $this->repository_model->insertRepository($info);
        if (!$return) {
            var_dump($this->repository_model->getErrors());
        }
    }

    function getRepository() {
        $repo = $this->repository_model->getAllRepository();
        print_r($repo);
    }

    function updateRepository() {
        $info = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2', 'username' => 'something', 'password' => 'password');
        $newInfo = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2/new', 'username' => 'newsomething', 'password' => 'newpassword');
        $result = $this->repository_model->updateRepository($info, $newInfo);
        if (!$result) {
            var_dump($this->repository_model->getErrors());
        }
    }

    function deleteRepository() {

        $info = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2', 'username' => 'something', 'password' => 'password');
        $result = $this->repository_model->deleteRepository($info);
        if (!$result) {
            var_dump($this->repository_model->getErrors());
        }
    }


    

}

?>
