
<?php
cfpr_header("knowledge bank","normal");

$search = $_POST['search_string'];
$topic = $_GET['topic'];


if ($search == "")
   {
   $id = $_GET['pid'];

   if (!$id)
      {
      $id = 935;
      }
   
   if ($topic)
     {
       // Could do: $search = $topic;
       
       $id = cfpr_get_pid_for_topic("body_constraints",$topic);
     }
    
    echo "<div id=\"image\">";
    include("/srv/www/htdocs/graphs/$id.map");
    echo "</div>";
    
    #$topic = cfpr_show_topic($id);
    #echo "$topic";
    
#    echo "<a href=\"new_topic.php\" type=\"submit\"><button>Propose a new topic</button></a>";
#    echo "<button>Propose an insight or association</button>";
#    echo "<button>Add a document reference to this topic</button>";
    
    $hits = cfpr_show_topic_hits($id);
    
    echo "<p>$hits<p>";
    
    $leads = cfpr_show_topic_leads($id);
    
    echo "<p>$leads";
    
    $cat = cfpr_show_topic_category($id);
    
    echo "<p>$cat";
  }
else
  {
    $ret = cfpr_search_topics($search,true);
    echo "$ret";
  }

cfpr_footer();
?>
          
