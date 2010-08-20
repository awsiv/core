
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
    
    #$topic = cfpr_show_topic($pid);
    #echo "$topic";
    
#    echo "<a href=\"new_topic.php\" type=\"submit\"><button>Propose a new topic</button></a>";
#    echo "<button>Propose an insight or association</button>";
#    echo "<button>Add a document reference to this topic</button>";

    echo "<table><tr><td valign=\"top\">";    

    echo "<div id=\"image\">";
    echo "<h2>Copernicus local cluster view</h2>";

    $docroot = cfpr_docroot();

    include("$docroot/graphs/$pid.map");
    echo "</div><br>";

    echo "<div id=\"image\">";
    if (file_exists("$docroot/graphs/influence_$pid.map"))
       {
       echo "<h2>Copernicus influence channel view</h2>";
       include("$docroot/graphs/influence_$pid.map");
       }

    echo "</div>";

    echo "</td><td>";    

    $hits = cfpr_show_topic_hits($pid);
    
    echo "<p>$hits<p>";
    
    $leads = cfpr_show_topic_leads($pid);
    
    echo "<p>$leads";
    
    $cat = cfpr_show_topic_category($pid);
    
    echo "<p>$cat";

    echo "</td></tr></table>";

  }
else
  {
    $ret = cfpr_search_topics($search,true);
    echo "$ret";
  }

cfpr_footer();
?>
          
