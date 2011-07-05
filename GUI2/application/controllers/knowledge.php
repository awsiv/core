<?php

class Knowledge extends Cf_Controller {

    function __construct() {
        parent::__construct();
        $this->load->helper('form');
        $this->load->library('table', 'cf_table');
    }

    function index() {
        $bc = array(
            'title' => 'Library',
            'url' => 'knowledge/index',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => "Cfengine Mission Portal - overview",
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
            'title' => 'Docs',
            'url' => 'knowledge/docs',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => "Cfengine Mission Portal - overview",
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


        $data = array();
        $data['searchData'] = array();

        $search = isset($getparams['search']) ? urldecode($getparams['search']) : $this->input->post('search');
        $data['search'] = trim($search);
        if ($data['search']) {
            $searchJson = cfpr_search_topics($search, true);
            $data['searchData'] = json_decode(utf8_encode($searchJson), TRUE);
            $this->load->view('/knowledge/search', $data);
        } else {
            // search for default manuals
            $pid = cfpr_get_pid_for_topic("any", "manuals");
            $searchJson = cfpr_show_topic_hits($pid);
            $data['searchData'] = json_decode(utf8_encode($searchJson), TRUE);
            $this->load->view('/knowledge/searchmanual', $data);
        }
    }

    function knowledgemap() {

        // add a js file
        $this->carabiner->js('jit/jit-yc.js');
        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));
        $this->carabiner->css('tabs-custom.css');

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
            'title' => 'Knowledge Map',
            'url' => $breadcrumbs_url,
            'isRoot' => false,
            'replace_existing' => true
        );
        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'search' => $search,
            'topic' => $topic,
            'pid' => $pid,
            'title' => "Cfengine Mission Portal - Knowledge bank",
            'title_header' => "Knowledge bank",
            'breadcrumbs' => $this->breadcrumblist->display(),
        );

        $graphdata = cfpr_get_knowledge_view($pid, '');
        $data['graphdata'] = ($graphdata);


        $topicDetail = cfpr_show_topic($pid);
        $topicsData = cfpr_show_topic_hits($pid);
        $topicLeads = cfpr_show_topic_leads($pid);
        $topicCategory = cfpr_show_topic_category($pid);

        // json decode the datas
        $data['topicDetail'] = json_decode(utf8_encode($topicDetail), true);
        $data['topicHits'] = json_decode(utf8_encode($topicsData), true);
        $data['topicLeads'] = json_decode(utf8_encode($topicLeads), true);
        $data['topicCategory'] = json_decode(utf8_encode($topicCategory), true);
        //$this->template->set('injected_item', implode("", $scripts));
        $this->template->load('template', 'knowledge/knowledge', $data);
    }

    function knowledgeSearch() {

        // add a js file
        $this->carabiner->js('jit/jit-yc.js');
        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));




        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search']) ? urldecode($getparams['search']) : $this->input->post('search');
        $topic = isset($getparams['topic']) ? urldecode($getparams['topic']) : $this->input->post('topic');

        if ($topic) {
            $search = $topic;
        }


        $data = array(
            'search' => $search,
            'topic' => $topic,
            'title' => "Cfengine Mission Portal - Knowledge bank",
            'title_header' => "Knowledge bank search",
            'breadcrumbs' => $this->breadcrumblist->display(),
        );

        $searchJson = cfpr_search_topics($search, true);
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