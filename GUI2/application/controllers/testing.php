<?php
class Testing extends CI_Controller
{
	
	function Testing()
	 {
		 parent::__construct();
     
	 }

         function index(){
             
         }
	
	function mongodbtest()
	 {
	    $this->load->library('mongo_db');
		$output=$this->mongo_db->insert('app_logs',array(

		// Server Info
		'server_name'	=> $_SERVER['SERVER_NAME'],
		'server_ip'		=> (isset($_SERVER['SERVER_ADDR'])) ? $_SERVER['SERVER_ADDR'] : '0.0.0.0',
		'domain'		=> (!empty($_SERVER['HTTP_HOST'])) ? $_SERVER['HTTP_HOST'] : '',

		//User Info
		'user_agent'	=> (!empty($_SERVER['HTTP_USER_AGENT'])) ? $_SERVER['HTTP_USER_AGENT'] : '',
		'ip_address'	=> (!empty($_SERVER['REMOTE_ADDR'])) ? $_SERVER['REMOTE_ADDR'] : '',
		'uri'			=> (!empty($_SERVER['REQUEST_URI'])) ? $_SERVER['REQUEST_URI'] : '',
		'query_string'	=> (!empty($_SERVER['QUERY_STRING'])) ? $_SERVER['QUERY_STRING'] : '',

		'timestamp'		=> date('Y-m-d H:i:s'),
		'message'		=> "testing",
		'level'			=> "0",
		));
            echo $output;
	 }

         function mongodbselect()
         {
         $this->load->library('mongo_db');
         
         //$result = $this->mongo_db->select(array('uri','message'))->get('app_logs');
         //$this->mongo_db->where(array('group'=>'faculty'));
         $result=$this->mongo_db->get('onlineusers');
         //$result=$this->mongo_db->select(array('username'))->get_where('users',array('group'=>'admin'));
         //print_r($result) ;
         foreach ($result as  $docs) {
             //print_r($value)  ;
              $obj=(object)$docs;
             /*foreach ($docs as $key => $value) {
                 echo $key.$value;
                 echo '</br>';
             }*/
            /* echo $docs['uri'];

             */
           // echo $obj->user_name.' ' ;
             print_r ($docs);
            /* foreach ($obj->group as $group )
             {
                 echo $group.'</br>';
             }*/
           echo '</br>';   
           }
         }

         function mongodbinput()
         {
          $this->load->model('ion_auth_model_mongo');
          $result=$this->ion_auth_model_mongo->register('admin', 'admin','admin@cfengine.com',array('admin','developer','faculty'));
          echo $result;
         }
         
	 function mongodbupdate()
         {
          $this->load->library('mongo_db');
          $this->mongo_db->where(array('_id'=>new MongoId('4d36a0424a235a8024000000')));
          $result=$this->mongo_db->update('users',array('username'=>'sudhir'));
          print_r($result);
         }

	 function mysqltest()
	 {
	   $this->load->database();
	   //$dbhandle = mysql_connect("localhost", "root", "") 
           // or die("Unable to connect to MySQL");
           echo "Connected to MySQL<br>";

	 }

         function deleteonlineusers()
         {
             $this->load->library('mongo_db');
             $this->mongo_db->delete('onlineusers');
         }

         function logintest()
         {
             /*$this->load->config('ion_auth', TRUE);
             $this->identity_column = $this->config->item('identity', 'ion_auth');
             $this->load->library('mongo_db');
             $result=$this->mongo_db->select(array($this->identity_column, '_id', 'password','group'))
                           ->where(array($this->identity_column => 'sudhir','active' => 1))
                           ->limit(1)
                           ->get_object('users');
            echo $result->password;*/

              $this->load->library('ion_auth');
              $result=$this->ion_auth->login('administrator','password');
              echo $result;

         }

         function isadmintest()
         {
              $this->load->library('ion_auth');
              $result=$this->ion_auth->login('administrator','password');
              if($result)
                  echo $this->ion_auth->is_admin();

         }

         function get_users()
         {
             $this->load->library('ion_auth');
             $user=$this->ion_auth->get_user('4d36a0424a235a8024000000');
             //$result=$this->ion_auth->get_users_array();
             //print_r($user);
             
             print_r ($user->_id);
         }

         function remove_user()
         {
             $this->load->library('ion_auth');
             $result=$this->ion_auth->delete_user('4d3846c14a235a755f000008');
             print_r($result);

         }

         function test_group()
         {
             $this->load->library('ion_auth');
              //$result=$this->ion_auth->create_group(array('name'=>'manager'));
              $result=$this->ion_auth->get_groups();
              foreach($result as $group)
              {
                  echo $group['name'].' '.$group['_id'].'<br/>';
              }
         }

         function test_update()
         {
         $this->load->library('mongo_db');
         $this->mongo_db->where(array('group'=>'faculties'));
         $this->mongo_db->update('users',array('group.$' => "faculty"));
         $this->mongo_db->clear();
         $result=$this->mongo_db->get('users');
         print_r($result);
         }

         function update_remove()
         {
         $this->load->library('mongo_db');
         $this->mongo_db->where(array('group'=>'faculties'));
         $result=$this->mongo_db->update_remove_field('users',array('groups'=>1));
         print_r($result);
         }

         function test_update_group()
         {
             $this->load->model('ion_auth_model_mongo');
             $this->load->library('ion_auth');
              //$result=$this->ion_auth->create_group(array('name'=>'manager'));
              //$result=$this->ion_auth_model_mongo->upate_group('4d3b187a4a235a5366000015',array('name'=>'manager'));
              //echo $result.$this->ion_auth->messages();

             $result= $this->ion_auth_model_mongo->get_group($group->_id != $id);
             if($result_id !='4d3b187a4a235a5366000015')
                {
		  $this->ion_auth->set_error('group_creation_duplicate');
                }
              echo $this->ion_auth->errors();

         }

         function test_login_remmembered()
         {
             $this->load->helper('cookie');
             $this->load->library('mongo_db');
              $result = $this->mongo_db->select(array($this->identity_column,'_id', 'group'))
			      ->where(array('username' =>get_cookie('identity'),'remember_code'=>get_cookie('remember_code')))
			      ->limit(1)
			      ->get_object('users');
              print_r($result);
         }

         function mail_test()
         {
            $this->load->library('email');
            $this->email->from('Sudhir.Pandey@cfengine.com', 'cfengine');
            $this->email->to('sudhir2pandey@gmail.com');

            $this->email->subject('Email Test');
            $this->email->message('Testing the email class.');

            $this->email->send();

            echo $this->email->print_debugger();
         }

         function mongoextensiontest()
         {
           $connection = new Mongo('localhost:27017');
           // Select a database
           $db = $connection->blog;
           //select tables
           //$posts = $db->posts;
           $document = array( 
                             'title' => 'test',
                                 );
           $result=$db->posts->insert($document);
           print_r($result);
         }

         function mongoclasstest()
         {
             $this->load->library('mongo_db');
         }

         function test_json()
         {
             $string=cfpr_show_green_hosts("green");
             $jsonarr=json_decode($string,true);
             $host=array();

             foreach ($jsonarr as $data)
             {
               //array_push(&$keys,$data['key'])
               //array_push(&$names,$data['id'])
                 $host[$data['key']]=$data['id'];
             }
            /* foreach ($jsonarr as $obj)
             {
                 
                     echo $obj['key']."<br/>";
                     echo $obj['id']."<br/>";
                 
             }
            */
             print_r($host);

         }

         function allhostjson()
         {
             $result=cfpr_select_reports(".*",100);
             print_r(json_decode($result));
         }

         function virtualdesktop()
         {
             $this->load->view('testviews/MUI_test/virtual_desktop');
         }

         function loadreports()
         {
             $this->load->helper('html');
             $this->load->view('testviews/MUI_test/reportslist');
         }

         function jsondata()
         {
          $reports=json_decode(cfpr_select_reports(".*",100));
          $adjacencies=array();
          $rootnode=array("id"=>"node0","name"=>"","data"=>array("\$type"=>"none"));
          $control=array();
          
          $i=1;
          foreach($reports as $report)
          {
             $node=array(
                'nodeTo'=>'node'.$i,
                 'data'=>array("\$type"=>'none')
             );
             $adjacencies[$i-1]=$node;
             $i++;
          }
          $rootnode['adjacencies']=$adjacencies;
          array_push($control, $rootnode);

          $i=1;
          foreach($reports as $report)
          {
             $node_property=array(
              'id'=>'node'.$i,
               'name'=>$report,
               'data'=>array("\$angularwidth"=>"20","\$color"=>"#bf0","\$height"=>70+$i),
                'adjacencies'=>array()
             );
              array_push($control, $node_property);
              $i++;
              echo $this->_rand_colorCode().'<br />';
          }

          //print_r($nodelist);
          //print_r(json_encode($control));
         }

         function _rand_colorCode(){
            $r = dechex(mt_rand(0,255)); // generate the red component
            $g = dechex(mt_rand(0,255)); // generate the green component
            $b = dechex(mt_rand(0,255)); // generate the blue component
            $rgb = $r.$g.$b;
            if($r == $g && $g == $b){
            $rgb = substr($rgb,0,3); // shorter version
            }
            if( strlen( $rgb ) == 4 )
            {
                $rgb = $rgb . rand(10,99);
            }
            else if( strlen( $rgb ) == 5 )
            {
               $rgb = $rgb . rand(0,9);
            }
            return '#'.$rgb;
        }

        function software_installed_autocomplete($column)
        {
            $data=cfpr_report_software_in(NULL,NULL,NULL,NULL,true,NULL,NULL,NULL);
            $decoded_data=json_decode($data);
            $column_index=$decoded_data->meta->header->$column;
            $column=array();
            foreach ($decoded_data->data as $rows)
            {
              array_push($column,$rows[$column_index]);
            }
            $unique_value=array_unique($column);
            
            if(is_array($unique_value))
            {
                $val=array_values($unique_value);
                echo json_encode($val);
            }
            else
            {
                echo json_encode($unique_value);
            }
        }

        function host_availabe()
        {
            $result= json_decode(cfpr_select_hosts("none", ".*", 100));
            $allhosts=array();
            if (count($result) > 0) {
            foreach ($result as $cols) {
                $allhosts[$cols->id]=$cols->key;
            }
            }
            print_r($allhosts);
        }

      function newui()
      {
          //$this->view->load('testviews/indexnew');
         $this->lang->load('cf_message');
         $this->load->library(array('breadcrumb','breadcrumblist'));
           $bc = array(
            'title' => 'Cfengine mission Portal',
            'url' => 'welcome/index',
            'isRoot' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);
        $this->data=array(
                     'breadcrumbs' => $this->breadcrumblist->display()
        );
        $this->load->view('testviews/indexnew',$this->data);
      }

      function filterhost()
      {
          $this->load->view('testviews/test');
      }

      function cfclasses()
      {
         //$this->load->library('session');
         //$arr=json_decode(cfpr_class_cloud($this->session->userdata('lastclasslist')));
          echo cfpr_report_classes(NULL,NULL,true,NULL,NULL,NULL);
         ///echo json_encode($arr->classes);
      }


      function listHost() {

          var_dump($_POST);
      }
      function buildfilter($recentfilter)
      {
         //function call applying recent filter;
          $this->load->library('session');
          $listhost="sds";
          $this->session->set_userdata('resulthostlist',$listhost);
          if(is_set($this->session->userdata('resulthostlist')))
          {
           //function call
           $classes="";
          }
          else
          {
          //function call
           $classes="";
          }
          return $classes;
          
      }

      function classCloud(){
         echo base_url().'/logs/onlineusers.tmp';
      }
function goals()
{
   $goals=json_decode(cfpr_list_business_goals());
   foreach ($goals as $goal)
   {
       echo $goal->desc;
   }
}

function license() {
        $expirydate=strtotime(cfpr_getlicense_expiry());
        $startDate=cfpr_getlicense_installtime();
        //echo date('D F d h:m:s Y',cfpr_getlicense_installtime())."\n";
        $datediff = $expirydate - $startDate ;
        $totaldays=floor($datediff/(60*60*24));
        echo $totaldays."<br />";
        echo time() ."<br/>";
        echo date('D F d h:m:s Y',time());
        $dayspassed=floor((time()-$startDate)/(60*60*24));
        //echo $dayspassed;
        //$pbarvalue=floor(($dayspassed/$totaldays)*100);
        //echo $pbarvalue;
    }

    function topn()
    {
         print_r(json_decode(cfpr_top_n_hosts("compliance", 1000,10,1),true));
        

    }
        
}