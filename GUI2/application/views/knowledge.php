<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>

<?php 
if ($search == "")
	   {
	   if ($topic)
		  {
		  $pid = cfpr_get_pid_for_topic("body_constraints",$topic);
		  }
	   if ($pid == 0 && $topic)
		  {
		  $ret = cfpr_search_topics($topic,true);
		  echo "$ret";
		  ?>
		  <script type="text/javascript">
		  $(document).ready(function() { 
		  $('div#disambig').addClass('pagepanel');
          $('div#disambig').find('h2').addClass('panelhead');
          $('div#disambig').find('ul').addClass('panelcontent');
		  });
		  </script>
		  </div>
		  <?php
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
		$path_graph=get_graphdir();
		if (file_exists("$path_graph$pid.map"))
		   {
			 include("$path_graph$pid.map");  
		   }
		 else
		 {
			echo "<span>This is an orphan topic. It is has currently no associations, and thus a topic map can not be built.\n";
                        echo " <br> For more information see <a href=\"/docs/cf3-tutorial.html#Knowledge-Management\">knowledge management</a>.</span>";
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
  <script type="text/javascript" src="<?php echo get_scriptdir()?>jquery.fancybox-1.3.1.js"></script>
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

