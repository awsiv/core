<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("knowledge bank","normal");
?>
<link href="css/jquery.fancybox-1.3.1.css" rel="stylesheet" media="screen" />

<?php
cfpr_menu("Knowledge bank");
?>
        <div id="tabpane">

<?php 
$search = $_POST['search'];
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
		  ?>
		  </div>
		  <script type="text/javascript">
		  $(document).ready(function() { 
		  $('div#disambig').addClass('pagepanel');
          $('div#disambig').find('h2').addClass('panelhead');
          $('div#disambig').find('ul').addClass('panelcontent');
		  });
		  </script>
		  <?php
		  include 'footer.php';
		  return;
		  }
	   else if ($pid == 0)
		  {
		  $pid = cfpr_get_pid_for_topic("","system policy");
		  }
		 echo "<div class=\"grid_7\">";
		 echo "<div class=\"panel\">";    
		 echo "<div class=\"panelhead\">Copernicus local cluster view</div>";
		 echo "<div class=\"panelcontent\">";
		 echo "<div class=\"imageviewer\"><div id=\"knowledge_pic\">";
		   
		$docroot = cfpr_docroot();
		if (file_exists("$docroot/graphs/$pid.map"))
		   {
			 include("$docroot/graphs/$pid.map");  
		   }
		 else
		 {
			echo "<span>No maps availabe for this topic.</span>";
		 }
		echo "</div></div></div></div>";
	
		if (file_exists("$docroot/graphs/influence_$pid.map"))
		   {
		   echo "<div class=\"panel\"><div class=\"panelhead\">Copernicus influence channel view</div>";
		   echo "<div class=\"panelcontent\">";
		   echo "<div class=\"imageviewer\"><div id=\"influence_knowledge_pic\">";
		   include("$docroot/graphs/influence_$pid.map");
		   echo "</div></div></div></div>";
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
	  // $target=$('div.panelcontent img');
       //$target.attr('src','/'+$target.attr('src'));
       styleit('div#occurrences')
	   styleit('div#associations');
	   styleit('div#others');
	    $('div#disambig').addClass('pagepanel');
        $('div#disambig').find('h2').addClass('panelhead');
        $('div#disambig').find('ul').addClass('panelcontent');
	 $('.imageviewer').dblclick(function()
		{
					$.fancybox({
		 //'orig' : $(this),
		 'padding' : 0,
		 'href' : '#'+$(this).find('div').attr('id'), 'title' : 'Knowledge Map',
		 'transitionIn' : 'elastic',
		 'transitionOut' : 'elastic'
		}); 
		});
	});
    function styleit(element)
	   {
	   $elem=$(element);
	   $elem.addClass('panel');
       $elem.find('h2').addClass('knwlgpanelhead');
       $elem.find('ul').addClass('panelcontent');
	   }
</script>
        
<?php
cfpr_footer();
?>