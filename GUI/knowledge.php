
<?php
cfpr_header("knowledge bank","normal");

$search = $_POST['search_string'];
$topic = $_GET['topic'];


if ($search == "")
   {
   $pid = $_GET['pid'];

   if ($topic)
      {
       // Could do: $search = $topic;
       
      $pid = cfpr_get_pid_for_topic("body_constraints",$topic);
      }

   if ($pid == 0 && $topic)
      {
      $ret = cfpr_search_topics($topic,true);
      echo "$ret";
      return;
      }
   else if ($pid == 0)
      {
      $pid = cfpr_get_pid_for_topic("","system policy");
      }
    
    echo "<div id=\"image\">";
    include("/srv/www/htdocs/graphs/$pid.map");
    echo "</div>";
    
    #$topic = cfpr_show_topic($pid);
    #echo "$topic";
    
#    echo "<a href=\"new_topic.php\" type=\"submit\"><button>Propose a new topic</button></a>";
#    echo "<button>Propose an insight or association</button>";
#    echo "<button>Add a document reference to this topic</button>";
    
    $hits = cfpr_show_topic_hits($pid);
    
    echo "<p>$hits<p>";
    
    $leads = cfpr_show_topic_leads($pid);
    
    echo "<p>$leads";
    
    $cat = cfpr_show_topic_category($pid);
    
    echo "<p>$cat";
  }
else
  {
    $ret = cfpr_search_topics($search,true);
    echo "$ret";
  }

cfpr_footer();
?>
          
