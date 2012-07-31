<?php

Abstract class CI_BaseTestCase extends CI_TestCase
{

    // --------------------------------------------------------------------
    var $mongoInstance = null;

    public function __construct()
    {

        $this->mongoInstance = new Mongo("localhost:" . MONGO_PORT);
        parent::__construct();
    }

    // --------------------------------------------------------------------

    public function setUp()
    {

        $this->loadFixtures();
    }

    // --------------------------------------------------------------------

    public function tearDown()
    {
        $fixtures = array(
            'hosts' => 'cfreport',
            'promises_exp' => 'cfreport',
            'promises_unexp' => 'cfreport',
            'roles' => 'cfreport',
            'settings' => 'cfreport',
            'users' => 'cfreport',
            'monitoring_mg' => 'cfreport',
            'custom_search' => 'phpcfengine',
            'trackers' => 'phpcfengine'
        );

        foreach ($fixtures as $collection => $db)
        {
            $mongodb = $this->mongoInstance->$db;
            $mongoCollection = $mongodb->$collection;
            $res = $mongoCollection->drop();
        }
    }

    protected function loadFixtures()
    {
        $fixtures = array(
             'settings' => 'cfreport',
            'roles' => 'cfreport',
            'users' => 'cfreport',
            'hosts' => 'cfreport',
            'promises_exp' => 'cfreport',
            'promises_unexp' => 'cfreport',
            'monitoring_mg' => 'cfreport',
            'custom_search' => 'phpcfengine'
        );

        foreach ($fixtures as $collection => $db)
        {
            $mongodb = $this->mongoInstance->$db;
            $mongoCollection = $mongodb->$collection;
            $fileName = TEST_MAIN_DIR . "/fixtures/" . $db . "." . $collection . '.json';
            $fileContent = file_get_contents($fileName);


            $c = json_decode($fileContent, true);
            if (!is_array($c)) {
              // var_dump($fileContent);
                die('no array for '.$collection);
            }
            $insertData = $c;
            $mongoCollection->drop();

            if ($collection === 'monitoring_mg') {

            }
                $res = $mongoCollection->batchInsert($insertData,array('continueOnError' => false,'fsync'=>true));
        }



        // read the file
    }

}

// EOF