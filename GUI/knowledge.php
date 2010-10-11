<?php
include 'header.php';
?>
<link href="css/jquery.fancybox-1.3.1.css" rel="stylesheet" media="screen" />
      <div id="nav">
             <ul class="grid_10">
              <li><a href="index.php">SUMMARY</a></li>
              <li><a href="helm.php">PLANNING</a></li>
              <li><a href="status.php">STATUS</a></li>
              <li><a href="knowledge.php" class="current">LIBRARY</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             Login:Max Manus
             </span>
             <div class="clearleft"></div>
        </div>
        
        <div id="tabpane">
         <div class="grid_7">
        <?php 
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
		  
		 echo "<div class=\"panel\">";    
		 echo "<div class=\"panelhead\">Copernicus local cluster view</div>";
		 echo "<div class=\"panelcontent\">";
		 echo "<div class=\"imageviewer\"><div id=\"knowledge_pic\">";
		   
		$docroot = cfpr_docroot();
		include("$docroot/graphs/$pid.map");
		echo "</div></div></div></div>";
	
		if (file_exists("$docroot/graphs/influence_$pid.map"))
		   {
		   echo "<div class=\"panel\"><div class=\"panelhead\">Copernicus influence channel view</div>";
		   echo "<div class=\"panelcontent\">";
		   include("$docroot/graphs/influence_$pid.map");
		   echo "</div></div>";
		   }
	
		echo "</div><div class=\"grid_5\">";    
	
		$hits = cfpr_show_topic_hits($pid);
		
		echo "<p>$hits<p>";
		
		$leads = cfpr_show_topic_leads($pid);
		
		echo "<p>$leads";
		
		$cat = cfpr_show_topic_category($pid);
		
		echo "<p>$cat";
	
		echo "</div><div class=\"clear\"></div>";
	
	  }
	else
	  {
		$ret = cfpr_search_topics($search,true);
		echo "$ret";
	  }
		?>
        
        </div>
  <script type="text/javascript" src="scripts/jquery.fancybox-1.3.1.js"></script>
  <script type="text/javascript">
	$(document).ready(function() { 
	   $target=$('div.panelcontent img');
       $target.attr('src','/'+$target.attr('src'));
       styleit('div#occurrences')
	   styleit('div#associations');
	   styleit('div#others');
	 $('.imageviewer').dblclick(function()
		{
			$.fancybox({
 //'orig' : $(this),
 'padding' : 0,
 'href' : "#knowledge_pic", 'title' : 'Lorem ipsum dolor sit amet',
 'transitionIn' : 'elastic',
 'transitionOut' : 'elastic'
}); 
		});
	});
    function styleit(element)
	   {
	   $elem=$(element);
	   $elem.addClass('panel');
       $elem.find('h2').addClass('panelhead');
       $elem.find('ul').addClass('panelcontent');
	   }
</script>
        
 <?php
include 'footer.php';
?>