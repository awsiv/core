<?php

class knowledge_model extends Cf_Model
{

    /**
     * 
     * @param string $username
     * @param string $search search term
     * @param bool $flag
     * @return array 
     */
    function searchTopics($username, $search, $flag = true)
    {
        try
        {
            $rawdata = cfpr_search_topics($username, $search, $flag);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Gets the data for certain topics
     * @param string $username
     * @param string $pid pid of the topic
     * @return array
     * @throws Exception 
     */
    function showTopicHits($username, $pid)
    {
        try
        {
            $rawdata = cfpr_show_topic_hits($username, $pid);
            $data = $this->checkData($rawdata);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Get the Knowledge map view
     * @param type $username
     * @param int $pid  topic id
     * @return JSON json representation of the knowledge map
     * 
     */
    function getKnowledgeView($username, $pid)
    {

        try
        {
            $result = cfpr_get_knowledge_view($username, $pid, '');
            $data = $this->checkData($result);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $result; // we need the JSON representation
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Returns a pid for the topic given
     * @param string $username
     * @param string $context
     * @param string $topic
     * @return int
     * @throws Exception 
     */
    function getPidForTopic($username, $context, $topic)
    {

        try
        {
            return cfpr_get_pid_for_topic($username, $context, $topic);
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Topics for knowledge map
     * @param string $username
     * @param int $pid
     * @return array 
     * @throws Exception 
     */
    function showTopics($username, $pid)
    {
        try
        {
            $result = cfpr_show_topic($username, $pid);
            $data = $this->checkData($result);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }
    
    
     /**
     * Topics for knowledge map with topic name
     * @param string $username
     * @param int $topicname
     * @return array 
     * @throws Exception 
     */
    function showTopicLeadsWithName($username, $topicname)
    {
        try
        {
            $result = cfpr_show_all_context_leads($username, $topicname);
            $data = $this->checkData($result);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }


    /**
     * Topics for knowledge map
     * @param string $username
     * @param int $pid
     * @return array 
     * @throws Exception 
     */
    function showTopicLeads($username, $pid)
    {
        try
        {
            $result = cfpr_show_topic_leads($username, $pid);
            $data = $this->checkData($result);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * Topic category for knowledge map
     * @param string $username
     * @param int $pid
     * @return array 
     * @throws Exception 
     */
    function showTopicCategory($username, $pid)
    {
        try
        {
            $result = cfpr_show_topic_category($username, $pid);
            $data = $this->checkData($result);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

    /**
     * 
     */
    function listDocuments($path = '')
    {

        try
        {
            $result = cfpr_list_documents($path);
            $data = $this->checkData($result);
            if (is_array($data) && $this->hasErrors() == 0)
            {
                return $data;
            }
            else
            {
                throw new Exception($this->getErrorsString());
            }
        }
        catch (Exception $e)
        {
            log_message('error', $e->getMessage() . " " . $e->getFile() . " line:" . $e->getLine());
            throw $e;
        }
    }

}

?>
