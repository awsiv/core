<?php

class Knowledge extends Cf_Controller
{

    private $username = "";

    function __construct()
    {
        parent::__construct();
        $this->load->helper('form');
        $this->load->library('table', 'cf_table');
        $this->load->model('knowledge_model');
        $this->username = $this->session->userdata('username');
    }

    /**
     * Checks if the current user has access to use the functionality
     * if rbac is ON and if the user is not admin then dont give them access to knowledge map
     */
    function _checkAccess()
    {
        $rbac = $this->setting_lib->get_rbac_setting();
        $isAdmin = $this->ion_auth->is_admin();
        if ($rbac && !$isAdmin)
        {
            $errorMessage = $this->lang->line('knowledge_access_denied');
            if (is_ajax())
            {
                show_error_custom($errorMessage, 403);
            }
            else
            {
                show_error($errorMessage, 403);
            }
        }
        return;
    }

    function index()
    {
        $bc = array(
            'title' => $this->lang->line('breadcrumb_library'),
            'url' => 'knowledge/index',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_library'),
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
    function docs()
    {


        $bc = array(
            'title' => $this->lang->line('breadcrumb_docs'),
            'url' => 'knowledge/docs',
            'isRoot' => false
        );

        $this->breadcrumb->setBreadCrumb($bc);

        $data = array(
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_docs'),
            'title_header' => "Docs",
            'breadcrumbs' => $this->breadcrumblist->display()
        );

        $root = getcwd();

        $root = $root . '/docs/';
        try
        {
            $data['docs'] = $this->knowledge_model->listDocuments($root);
        }
        catch (Exception $e)
        {
            show_error($e->getMessage(), 500);
        }
        $this->template->load('template', 'knowledge/docs', $data);
    }

    function topicFinder()
    {

        $this->_checkAccess();

        $data = array();
        $data['searchData'] = array();

        $search = isset($getparams['search']) ? urldecode($getparams['search']) : $this->input->post('search');
        $data['search'] = trim($search);
        if (!$data['search']) {
            $data['search'] = 'questions::.*'; // default topic
        }
        try
        {
                $data['searchData'] = $this->knowledge_model->searchTopics($this->username, $data['search'], true);
                $this->load->view('/knowledge/search_result', $data);

        }
        catch (Exception $e)
        {
            show_error_custom($e->getMessage(), 500);
        }
    }

    function knowledgemap()
    {

        $this->_checkAccess();
        $this->carabiner->css(
                array(
                    array('view/engineering.css'),
                    array('astrolabe.css'),
                    array('jScrollPane.css')
                )
        );

        // add a js file
        $this->carabiner->js('jit/jit-yc.js');
        $this->carabiner->js('jquery.cookie.js');
        $this->carabiner->js('jScrollPane.js');
        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));
        $this->carabiner->css('tabs-custom.css');
        $this->load->model('stories_model');
        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search']) ? $getparams['search'] : $this->input->post('search');
        $topic = isset($getparams['topic']) ? $getparams['topic'] : $this->input->post('topic');
        $pid = isset($getparams['pid']) ? $getparams['pid'] : $this->input->post('pid');
        // chech for integer
        try
        {
            $pid = intval($pid, 10);
            if (!$pid)
            {
                $pid = $this->knowledge_model->getPidForTopic($this->username, "", "system policy");
            }
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
                'pagingParam' => $breadcrumbs_url
            );

            $data['graphdata'] = $this->knowledge_model->getKnowledgeView($this->username, $pid);

            $data['topicHits'] = $this->knowledge_model->showTopicHits($this->username, $pid);
            //$data['topicLeads'] = $this->knowledge_model->showTopicLeads($this->username, $pid);

            $data['topicDetail'] = $this->knowledge_model->showTopics($this->username, $pid);

            $data['topicLeads'] = $this->knowledge_model->showTopicLeadsWithName($this->username, $data['topicDetail']['topic']);


            $data['topicCategory'] = $this->knowledge_model->showTopicCategory($this->username, $pid);

            $data['showLeads'] = (!is_array($data['topicLeads']) || empty($data['topicLeads'])) ? false : true;
            $data['showTopicHits'] = (!is_array($data['topicHits']) || empty($data['topicHits'])) ? false : true;
            $data['showSameContext'] = (!is_array($data['topicCategory']['other_topics']) || empty($data['topicCategory']['other_topics'])) ? false : true;
            $data['showSubTopics'] = (!is_array($data['topicCategory']['topic']['sub_topics']) || empty($data['topicCategory']['topic']['sub_topics'])) ? false : true;

            $data['currentPage'] = isset($getparams['page']) ? intval($getparams['page'], 10) : 1;
            $data['number_of_rows'] = isset($getparams['rows']) ? intval($getparams['rows'], 10) : 15;

            //for story generation
            $data['stories'] = $this->stories_model->getStoryById($pid);
            $data['showStory'] = is_array($data['stories']) ? true:false;
            $this->template->load('template', 'knowledge/knowledge', $data);
        }
        catch (Exception $e)
        {
            show_error($e->getMessage(), 500);
        }
    }

    function knowledgeSearch()
    {

        $this->_checkAccess();

        // add a js file
        $this->carabiner->js('jit/jit-yc.js');
        $jsIE = array('jit/Extras/excanvas.js');
        $this->carabiner->group('iefix', array('js' => $jsIE));




        $getparams = $this->uri->uri_to_assoc(3);
        $search = isset($getparams['search']) ? urldecode($getparams['search']) : $this->input->post('search', true);
        $topic = isset($getparams['topic']) ? urldecode($getparams['topic']) : $this->input->post('topic', true);

        if ($topic)
        {
            $search = "^" . $topic . "$";
        }


        $data = array(
            'search' => htmlspecialchars($search),
            'topic' => $topic,
            'title' => $this->lang->line('mission_portal_title') . " - " . $this->lang->line('breadcrumb_kw_bank'),
            'breadcrumbs' => $this->breadcrumblist->display(),
        );
        try
        {
            $data['searchData'] = $this->knowledge_model->searchTopics($this->username, strtolower($search));
        }
        catch (Exception $e)
        {
            show_error($e->getMessage(), 500);
        }
        // if only one result is found with id redirect to knowledge map else show list
        if (is_array($data['searchData']) && count($data['searchData']) == 1 && isset($data['searchData'][0]['id']))
        {
            $pid = $data['searchData'][0]['id'];
            redirect('/knowledge/knowledgemap/pid/' . $pid);
            exit;
        }

        $this->template->load('template', 'knowledge/search', $data);
    }

}

/* End of file welcome.php */
/* Location: ./system/application/controllers/welcome.php */
