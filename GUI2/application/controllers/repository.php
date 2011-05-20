<?php

class Repository extends CI_Controller {

    function __construct() {
        parent::__construct();
        $this->load->model('repository_model');
    }

    function insertRepository() {
        $info = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2', 'username' => 'something', 'password' => 'password');
        $return = $this->repository_model->insert_repository($info);
        if (!$return) {
            var_dump($this->repository_model->get_errors());
        } else {
            var_dump($return);
        }
    }

    function getRepository() {
        $repo = $this->repository_model->get_all_repository();
        foreach ($repo as $obj) {
            $info = array('userId' => $obj['userId'], 'password' => $obj['password']);
            $pass = $this->repository_model->decrypt_password($info);
            //var_dump($obj['password']);
            echo "Enc pass :: " . $obj['password'] . " original pass :: " . $pass;
            echo "<br>";
        }
    }

    function updateRepository() {
        $info = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2', 'username' => 'something', 'password' => 'password');
        $newInfo = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2/new', 'username' => 'newsomething', 'password' => 'newpassword');
        $result = $this->repository_model->update_repository($info, $newInfo);
        if (!$result) {
            var_dump($this->repository_model->get_errors());
        }
    }

    function deleteRepository() {

        $info = array('userId' => 'sudhir', 'repoPath' => 'http://test/test2', 'username' => 'something', 'password' => 'password');

        $result = $this->repository_model->delete_repository();
        if (!$result) {
            var_dump($this->repository_model->get_errors());
        }
    }

}

?>
