
<?php

cfpr_header("knowledge bank","normal");

#$id = 1491;

$id = 1493;

echo "<div id=\"image\">";
include("/srv/www/htdocs/graphs/$id.map");
echo "</div>";

#$topic = cfpr_show_topic($id);
#echo "$topic";

echo "<a href=\"new_topic.php\" type=\"submit\"><button>Propose a new topic</button></a>";
echo "<button>Propose an insight or association</button>";
echo "<button>Add a document reference to this topic</button>";

$hits = cfpr_show_topic_hits($id);

echo "<p>$hits<p>";

$leads = cfpr_show_topic_leads($id);

echo "$leads";


cfpr_footer();
?>
          
