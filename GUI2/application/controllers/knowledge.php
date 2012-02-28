<?php

class Knowledge extends Cf_Controller {

    private $username = "";

    function __construct() {
        parent::__construct();
        $this->load->helper('form');
        $this->load->library('table', 'cf_table');
        $this->username = $this->session->userdata('username');
    }

    /**
     * Checks if the current user has access to use the functionality 
     * if rbac is ON and if the user is not admin then dont give them access to knowledge map 
     */
    function _checkAccess() {
        $rbac = $this->settings_model->app_settings_get_item('rbac');
        $isAdmin = $this->ion_auth->is_admin();
        if ($rbac && !$isAdmin) {
            if (is_ajax()) {                
                show_error_custom("you dont have permission to access the knowledgemap.", 403);
            } else {
                show_error("you dont have permission to access the knowledgemap.", 403);
            }
        }
        return;
    }

    function index() {
        $bc = array(
            'title' => $this->lang->line('breadcrumb_library'),
            'url' => 'knowledge/index',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => $this->lang->line('mission_portal_title')." - ".$this->lang->line('breadcrumb_library'),
            'title_header' => "Library",
            'breadcrumbs' => $this->breadcrumblist->display()
        );
        $requiredjs = array(
            array('widgets/notes.js'),           
            array('widgets/topicfinder.js'),
        );
        $this->carabiner->js($requiredjs);



        $this->template->load('template', 'knowledge/index', $data);
    }

    /**
     * Doc links pages
     */
    function docs() {


        $bc = array(
            'title' => $this->lang->line('breadcrumb_docs'),
            'url' => 'knowledge/docs',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => $this->lang->line('mission_portal_title')." - ".$this->lang->line('breadcrumb_docs'),
            'title_header' => "Docs",
            'breadcrumbs' => $this->breadcrumblist->display()
        );

        $root = getcwd();
        
        $root = $root . '/docs/';
        $docdata = cfpr_list_documents($root);

        $data['docs'] = json_decode(utf8_encode($docdata), true);

        $this->template->load('template', 'knowledge/docs', $data);
    }

    function topicFinder() {

        $this->_checkAccess();

        $data = array();
        $data['searchData'] = array();

        $search = isset($getparams['search']) ? urldecode($getparams['search']) : $this->input->post('search');
        $data['search'] = trim($search);
        if ($data['search']) {
            $searchJson = cfpr_search_topics($this->username, $search, true);
            $data['searchData'] = json_decode(utf8_encode($searchJson), TRUE);
            $this->load->view('/knowledge/search_result', $data);
        } else {
            // search for default manuals
            $pid = cfpr_get_pid_for_topic($this->username, "any", "manuals");
            $searchJson = cfpr_show_topic_hits($this->username, $pid);
            $data['searchData'] = json_decode(utf8_encode($searchJson), TRUE);
            $this->load->view('/knowledge/searchmanual', $data);
        }
    }

    function knowledgemap() {

        $this->_checkAccess();

        // add a js file
        $this->carabiner->js('jit/jit-yc.js');
        $this->carabiner->js('jquery.cookie.js');
        $this->carabiner->js('jquery.jsPlumb-1.3.3-all-min.js');
        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));
        $this->carabiner->css('tabs-custom.css');
        $this->load->model('stories_model');

        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search']) ? $getparams['search'] : $this->input->post('search');
        $topic = isset($getparams['topic']) ? $getparams['topic'] : $this->input->post('topic');
        $pid = isset($getparams['pid']) ? $getparams['pid'] : $this->input->post('pid');
        // chech for integer
        $pid = intval($pid, 10);
        if (!$pid)
            $pid = cfpr_get_pid_for_topic("", "system policy");

        $breadcrumbs_url = "knowledge/knowledgemap/pid/$pid";
        $bc = array(
            'title' => $this->lang->line('breadcrumb_kw_bank'),
            'url' => $breadcrumbs_url,
            'isRoot' => false,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'search' => $search,
            'topic' => $topic,
            'pid' => $pid,
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_kw_bank'),
            'breadcrumbs' => $this->breadcrumblist->display(),
        );

        $graphdata = cfpr_get_knowledge_view($this->username, $pid, '');
        $data['graphdata'] = ($graphdata);


        $topicDetail = cfpr_show_topic($this->username, $pid);
        $topicsData = cfpr_show_topic_hits($this->username, $pid);
        $topicLeads = cfpr_show_topic_leads($this->username, $pid);
        $topicCategory = cfpr_show_topic_category($this->username, $pid);

        // json decode the datas
        $data['topicDetail'] = json_decode($topicDetail, true);
        $data['topicHits'] = json_decode($topicsData, true);
        $data['topicLeads'] = json_decode($topicLeads, true);
        $data['topicCategory'] = json_decode(($topicCategory), true);

        $data['showLeads'] = (!is_array($data['topicLeads']) || empty($data['topicLeads'])) ? false : true;
        $data['showTopicHits'] = (!is_array($data['topicHits']) || empty($data['topicHits'])) ? false : true;
        $data['showSameContext'] = (!is_array($data['topicCategory']['other_topics']) || empty($data['topicCategory']['other_topics'])) ? false : true;
        $data['showSubTopics'] = (!is_array($data['topicCategory']['topic']['sub_topics']) || empty($data['topicCategory']['topic']['sub_topics'])) ? false : true;

        //for story generation
        if (is_constellation()) {
            $data['story'] = $this->stories_model->getStoryByName($data['topicDetail']['topic']);
            $stories = json_decode(utf8_encode($data['story']), true);
            $data['showStory'] = (!is_array($stories) || empty($stories['F'])) ? false : true;
        }
        $this->template->load('template', 'knowledge/knowledge', $data);
    }

    function knowledgeSearch() {

        $this->_checkAccess();

        // add a js file
        $this->carabiner->js('jit/jit-yc.js');
        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));




        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search']) ? urldecode($getparams['search']) : $this->input->post('search', true);
        $topic = isset($getparams['topic']) ? urldecode($getparams['topic']) : $this->input->post('topic', true);

        if ($topic) {
            $search = $topic;
        }


        $data = array(
            'search' => htmlspecialchars($search),
            'topic' => $topic,
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_kw_bank'),
            'breadcrumbs' => $this->breadcrumblist->display(),
        );

        $searchJson = cfpr_search_topics($this->username, strtolower($search), true);
        $data['searchJson'] = $searchJson;
        $data['searchData'] = json_decode(utf8_encode($searchJson), TRUE);

        // if only one result is found with id redirect to knowledge map else show list
        if (is_array($data['searchData']) && count($data['searchData']) == 1 && isset($data['searchData'][0]['id'])) {
            $pid = $data['searchData'][0]['id'];
            redirect('/knowledge/knowledgemap/pid/' . $pid);
            exit;
        }

        $this->template->load('template', 'knowledge/search', $data);
    }

}

/* End of file welcome.php */
/* Location: ./system/application/controllers/welcome.php */