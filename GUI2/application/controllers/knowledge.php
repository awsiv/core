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



        $this->template->load('template', 'knowledge/index', $data);
    }

    function knowledgemap() {

        // add a js file
        $this->carabiner->js('jit/jit-yc.js');
        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));
        $this->carabiner->css('tabs-bottom.css');

        $bc = array(
            'title' => 'Knowledge Map',
            'url' => 'knowledge/knowledgemap',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search']) ? $getparams['search'] : $this->input->post('search');
        $topic = isset($getparams['topic']) ? $getparams['topic'] : $this->input->post('topic');
        $pid = isset($getparams['pid']) ? $getparams['pid'] : $this->input->post('pid');

        if (!$pid)
            $pid = cfpr_get_pid_for_topic("", "system policy");

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

    function knowledgeSearch($topic='') {


        // add a js file
        $this->carabiner->js('jit/jit-yc.js');
        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));


        $bc = array(
            'title' => 'Knowledge Map',
           'url' => 'knowledge/knowledgemap',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search']) ? $getparams['search'] : $this->input->post('search');
        $topic = isset($getparams['topic']) ? urldecode($getparams['topic']) : $this->input->post('topic');

        if ($topic) {
            $search = $topic;
            // temp pid
            // have to redirect to knowledge Map ??
            $pid = cfpr_get_pid_for_topic("body_constraints", $topic);
            if ($pid) {
                redirect('welcome/knowledge/pid/' . $pid);
            }
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
        $this->template->load('template', 'knowledge/search', $data);
    }
}
/* End of file welcome.php */
/* Location: ./system/application/controllers/welcome.php */