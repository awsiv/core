<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
cfpr_header("Policy editor","none");
?>

<link href="css/jquery-ui-1.8.2.custom.css" rel="stylesheet" media="screen" />
    <div id="editor">
	<div id="leftPanel">
	 	<div class="ui-layout-content">
        <div id="Policies">
		  <div class="title">Nova Policies</div>
		    <div id="container_policies_id">
		    
		    </div>
		  </div>
		  <div id="menu">
			<a href="#" class="menuitem" id="new"><img class="icontext" src="images/new.png"/>New</a>
			  <a href="#" class="menuitem" id="save"><img class="icontext" src="images/save.png">Save</a>
			  <a href="#" class="menuitem" id="checksyntax"><img class="icontext" src="images/check_syntax_grey.png"/>Check syntax</a>
              <a href="#" class="menuitem" id="Checkout"><img class="icontext" src="images/checkout.png"/>Checkout</span></a>
			  <a href="#" class="menuitem" id="update"><img class="icontext" src="images/update.png"/>Update</span></a>
			  <a href="#" class="menuitem" id="Commit"><img class="icontext" src="images/commit.png"/>Commit</span></a>
              <a href="#" class="menuitem" id="svnlogs"><img class="icontext" src="images/commit_grey.png"/>Log</span></a>
		  </div>
		</div>
	</div>
	
	<div id="rightPanel">
	<div class="ui-layout-content">
	 <div id="tabs">
		  <ul>
			   <li>
			   <a href="#news">Welcome</a>
                           <!--<span class="ui-icon ui-icon-close">Remove Tab</span>-->
			   </li>
		  </ul>
		  <div id="news">
                  Welcome to the Cfengine Nova policy editor. Please click an existing policy file on the left, or "New" to start editing.
		  </div>
	 </div>

	</div>
</div>
</div> <!--hello-->

    <div title="Tab data" id="dialog" style="display:none">
            <form>
                <fieldset class="ui-helper-reset">
                    <label for="tab_title">File Name</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="tab_title" name="tab_title" />
                
                    <textarea class="ui-widget-content ui-corner-all" id="tab_content" name="tab_content"></textarea>
                    <input type="hidden" id="event"></input>
                </fieldset>
            </form>
        </div>
        
    <div title="Save File" id="sfdialog" style="display:none">
        <span></span>
        <input type="hidden" id="tobesaved" ></input>
        <input type="hidden" id="tobesaved_name"></input>
    </div> 
<div id="commitdlg" style="display:none;" title="Commit file" class="dialog">

            <form>
                <fieldset class="ui-helper-reset">
                    <label for="tab_title">User name:</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="username" />
                    <label for="tab_title">Password:</label>
                    <input type="password" class="ui-widget-content ui-corner-all" value="" id="password" />
                    <label for="tab_title" id="repolbl">Repository</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="repo" />
                    <label for="tab_title" id="commentlbl">Comment</label>
                    <textarea class="ui-widget-content ui-corner-all" id="comments" name="comments"></textarea>
                    <input type="hidden" id="event"></input>
                    <input type="hidden" id="operation" ></input>
                    <input type="hidden" id="datatype" ></input>
                    <input type="hidden" id="cmtfile" ></input>
                </fieldset>
            </form>

   </div>
   <div id="confirmation" style="display:none">
   </div>
   <div id="svnlogtable" style="display:none;" class="dialog">
   </div>
   <div id="checkoutconfirmation" style="display:none" class="dialog">
   </div>
   <div id="usedRepo" style="display:none">  
   </div>
<script src="scripts/Cfeditor/jquery.layout.min-1.2.0.js" type="text/javascript"></script>
<script src="scripts/Cfeditor/codemirror.js" type="text/javascript"></script>
<script src="scripts/Cfeditor/cf.js" type="text/javascript"></script>
<script src="scripts/jquery.jcryption-1.1.min.js" type="text/javascript"></script>
<script type="text/javascript" src="scripts/jquery.fancybox-1.3.1.js"></script>
 <script type="text/javascript">
 $(document).ready(function() {
	$("body").layout({
	defaults: {
	      fxName:               "slide"
	   ,  fxSpeed:               "slow"
	   ,  spacing_open:          14
	   }
	,  west: {
	      fxName:                "slide"
	   ,  size:                  250
	   ,togglerLength_closed:    21
	   ,togglerAlign_closed:     "top"
	   ,  spacing_closed:        8
	   ,  togglerLength_closed:  "100%"
	   ,  paneSelector:          "#leftPanel"
	   ,  fxSettings:             {easing:""}
	   }
	,  center: {
		   paneSelector:         "#rightPanel"
	   }
	,  north: {
		   
		   paneSelector:         "#header"
	   ,  size:                  40
	   ,  spacing_open:          0
	   ,  resizable:             false
	   ,  closeable:             false
	      
	   }
	,  south: {
		paneSelector:         "#footer"
	   ,  size:                  15
	   ,  spacing_open:          0
	   ,  resizable:             false
	   ,  closeable:             false
	   }
	});
	
	var tab_counter = 2;
		var code_editor_counter=0;//previously 1
		var tab_title_input="Untitled-";
		var tab_content_input="";
		var current_tab_content="";
		var current_tab_title="";
		var current_tab_index="";
		var current_tab_id="tabs-1";
		var cur_file_path="";
		$title_element="";
		var closetabs=false;
		var closetabindex="";
		
	var $tabs = $('#tabs').tabs({
	   tabTemplate: '<li><a href="#{href}">#{label}</a> <span class="ui-icon ui-icon-close">Remove Tab</span></li>',
	   add: function(event, ui) {
	   var tab_content =tab_content_input||'Tab '+tab_counter+' content.';
	   $(ui.panel).append(tab_content);
	   $tabs.tabs('select', '#' + ui.panel.id);
	   var h=$('div#rightPanel').height();
	   $("#tabs").css('height', h-15 );
	   $(".ui-tabs-panel").css('height', h-50 );
			},
	   select: function(event, ui) {
		//current_tab_content=$(ui.panel).children("textarea").html();
		current_tab_content=$(ui.panel).html();
		current_tab_title=$(ui.tab).html();
		current_tab_id=$(ui.tab).attr('href');
		current_tab_index=ui.index;
		$title_element=ui.tab;
		},
		fx: { 
			opacity: 'toggle',speed:'fast'
			},
		remove: function(event, ui) {
				current_tab_index=current_tab_index-1;
				tab_counter--;
			 }
		 });
	  
	  //adjusting the height of the tabs
	   var tab_height=$('div#rightPanel').height();
	   $("#tabs").css('height', tab_height-15 );
	   $(".ui-tabs-panel").css('height', tab_height-50 );
	   
	   $('<img src="images/ajax-loader.gif" id="spinner" />').css('position','absolute').hide().appendTo('body');
	   
	   
	   
	   //load policies from database into left hand side panel on list with id Policies_list
	var path = "policy/get_list.php";
	$("#container_policies_id").load(path,{dir: 'policies/'}, function(data){	
	});
	
	$('ul#policies_list_new li a').live('click',function(){
        openfile($(this).attr('rel'),$(this).attr('id'));
        $('#'+$(this).attr('id')).hide('slow');
	});
	
	function addTab(op) {
		var tab_title = tab_title_input || 'Tab '+tab_counter;
		if(op=='new')
			{	
			tab_title=tab_title+code_editor_counter;
			}
		$tabs.tabs('add', '#tabs-'+tab_counter, tab_title);
		tab_counter++;
		tab_title_input="Untitled-";
	   }
	   
	$('#new')
     .click(function() {
      tab_content_input='<textarea id="code'+code_editor_counter+'" cols="120" rows="30"></textarea>'+
    	                  '<input type="hidden" name="tabtype" value="codeEditor" />';              
    	 addTab('new');
    	 var editor = CodeMirror.fromTextArea('code'+code_editor_counter, {
    		    parserfile: ["cfsyntax.js", "parsecf.js"],
    		    stylesheet: "css/cfcolors.css",
    		    path: "scripts/Cfeditor/",
    		    lineNumbers: true,
    		    styleNumbers: styleLineNum,
    		    initCallback: function(e){initComplete(e); initResize(e); initChangeTracker(e);},
    		    onChange: contentChanged,
    		    continuousScanning: 500
    		  });
		  
		code_editor_counter++; 
		$('.CodeMirror-wrapping',current_tab_id).get(0).style.height=$(current_tab_id).get(0).style.height;
	});
	
	function openfile(file,link_id)
	 {
		 $.ajax({
	           type: "POST",
	           url: "policy/get_file_contents.php",
	           data:({'file_path':file}),
	           dataType:'json',
	           success: function(data){
	        	   tab_title_input=data.filename;
	        	   //cur_file_path=data.information.server_path;
		           tab_content_input='<input type="hidden" name="tabtype" value="codeEditor" /><input type="hidden" name="link" value="'+link_id+'" />'+
		                             '<textarea id="code'+code_editor_counter+'" cols="120" rows="30">'+data.content.replace(/\\n/g,"\n")+'</textarea>';
		                
		       	    addTab('existing');//adding the new tab
			       	 var editor = CodeMirror.fromTextArea('code'+code_editor_counter, {
			       		    parserfile: ["cfsyntax.js", "parsecf.js"],
			       		    stylesheet: "css/cfcolors.css", // you may have to adjust the path
			       		    path: "scripts/Cfeditor/",
			       		    lineNumbers: true,
			       		    styleNumbers: styleLineNum,
			       		    initCallback: function(e){initComplete(e); initResize(e); initChangeTracker(e);},
			       		    onChange: contentChanged,
			       		    continuousScanning: 500
			       		  });
		   		code_editor_counter++;
		   		$('.CodeMirror-wrapping',current_tab_id).get(0).style.height=$(current_tab_id).get(0).style.height;
		   		//addcontextmenu();//adding the context menu to the tab newly added
	             },
          error:function(data){
	            	 $confirmation.dialog({title: "Error"});
	            	 $confirmation.html('<span>Error! while performing request</span>'); 
	            	 $confirmation.dialog('open');   
	             }
	       }); 
	 }
	 
	 
	//respond to the tab closing event/removing event	  
	$('#tabs span.ui-icon-close').live('click', function() {
	 var index = $('li',$tabs).index($(this).parent());
	 var tab_id=$('a',$(this).parent()).attr("href");
	 var file_type=$("input[name='link']",tab_id).val();
	 var tabtype=$("input[name='tabtype']",tab_id).val();
	 var link=$("input[name='link']",tab_id).val();
	 var agent=jQuery.uaMatch(navigator.userAgent).browser;
     var tab_title=$('a',$(this).parent()).html();
     var presrevelinespace;
	 var newcontents;
	 if(tabtype=='codeEditor')//if the tab is of type editor and it should complete couple of things befor closing.
	 {
		 var tab_contents=$('iframe',tab_id).contents().find('.editbox').html();
		 if(agent=='msie')
		 {
			 presrevelinespace=tab_contents.replace(/<br(?:"[^"]*"['"]*|'[^']*'['"]*|[^'">])+>/gi,"\n");
			 presrevelinespace=presrevelinespace.replace(/&nbsp;/gi," ");
			 newcontents = presrevelinespace.replace(/<(?:"[^"]*"['"]*|'[^']*'['"]*|[^'">])+>/ig,"");
			 newcontents= newcontents.replace(/&gt;/gi,">");
		 }
		 else
		 {
			 presrevelinespace=tab_contents.replace(/<br[^>]*>/gi,"\n");
			 presrevelinespace=presrevelinespace.replace(/&nbsp;/gi," ");
			 newcontents = presrevelinespace.replace(/(<.*?>)/ig,"");
			 newcontents= newcontents.replace(/&gt;/gi,">");
		 }
		 if(file_type!=undefined)//tab is old so update necessary things
		 {
			 $.ajax({
			        type: "POST",
			        url: "policy/compare_file_contents.php",
			        data:({'file':tab_title,'newcontents':newcontents,'agent':agent}),
			        dataType:'json',
			        global: false,
			        success: function(data){
				        var status=data.status;
				        if(status=='changed')
				        { 
					       $('#tobesaved_name',$sfd).val(tab_title);
					       $('#tobesaved',$sfd).val(newcontents);
					       $('span',$sfd).html('The contents of the '+tab_title+' have been modified. Do you want to save the changes?');
				           $sfd.dialog('open');   
				        }
						else
						{
							$tabs.tabs('remove', closetabindex);
							closetabindex="";
			                closetabs=false;
						}
				        
			        }
			 });
			 $('#'+link).show('fast');
			
		 }
		 else//tab is new so dispay dialog before closing
		 {
			 $("#tab_title").val(tab_title);
			 $("#tab_content").html(newcontents);
			 $("#event").val('closing');
			 $dialog.dialog('open');
		 }
		 code_editor_counter--;
	 }
	// $tabs.tabs('remove', index);
	closetabs=true;
	closetabindex=index;
	 });
	 
	 
	 //dialogue to be displayed in different scenerio
	 var $dialog = $('#dialog').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 buttons: {
		 'Save': function() {
		 $.ajax({
           type: "POST",
           url: "policy/save_file_contents.php",
           data:({'file':$('#tab_title').val(), 'content':$('#tab_content').html(), 'filestats':'new'}),
           dataType:'json',
           success: function(data){
			   if(data.status)
			   {
        	   var id= $('ul#policies_list_new li').length+1;
               var append_html='<li class="file ext_txt"><a href="#" rel="'+data.path+'" id="policy_'+id+'">'+data.title+'</a></li>';
               $('#policies_list_new').append(append_html);
					 if($("#event").val()!="closing")
					 {
					   var append_html_tab='<input type="hidden" name="link" value="policy_'+id+'" />';
					   $(current_tab_id).append(append_html_tab);
					   $('a[href="'+current_tab_id+'"]',$tabs).html(data.title);
					   $('#policy_'+id).hide('slow');
					 }
					 else
					 {
						$('#policy_'+id).show('slow');
					 }
                }
				else
			  {
				closetabs=false; 
				$confirmation.dialog({title: "Error"});
	            $confirmation.html('<span>Error! occured'+data.msg+'</span>'); 
	            $confirmation.dialog('open'); 
			  }
		      }
            });
		 $(this).dialog('close');
		 },
		 'Cancel': function() {
		 $(this).dialog('close');
		 if(closetabs)
		   {
			  $tabs.tabs('remove', closetabindex);
			  closetabindex="";
			  closetabs=false;
		   }
		 }
		 },
		 open: function() {
		 $('#tab_title').focus();
		 $('#tab_content').hide();//hiding the contents of the tab.
		 },
		 close: function() {
		 
		 }
		 });
	 
	 //hooking the save link click event to perform selection of tab and contents before saving and decide the editing tab contains new/old item
	    $('#save')
	   .click(function() {
		 var tabtype=$("input[name='tabtype']",current_tab_id).val();
		 if(tabtype==undefined)
		 {
			 alert("Sorry! the contents cannot be saved");
			 return;
		 }
		var file_type=$("input[name='link']",current_tab_id).val();
		var agent=jQuery.uaMatch(navigator.userAgent).browser;
		var tab_contents=$('iframe',current_tab_id).contents().find('.editbox').html();
		var presrevelinespace;
		var newcontents;
		if(agent=='msie')
		 {
			 presrevelinespace=tab_contents.replace(/<br(?:"[^"]*"['"]*|'[^']*'['"]*|[^'">])+>/gi,"\n");
			 presrevelinespace=presrevelinespace.replace(/&nbsp;/gi," ");
			 newcontents = presrevelinespace.replace(/<(?:"[^"]*"['"]*|'[^']*'['"]*|[^'">])+>/ig,"");
			 newcontents= newcontents.replace(/&gt;/gi,">");
		 }
		 else
		 {
			 presrevelinespace=tab_contents.replace(/<br[^>]*>/gi,"\n");
			 presrevelinespace=presrevelinespace.replace(/&nbsp;/gi," ");
			 newcontents = presrevelinespace.replace(/(<.*?>)/ig,"");
			 newcontents= newcontents.replace(/&gt;/gi,">");
		 }

		 if(file_type!=undefined)//decide if its not the new file
		 {
		 $.ajax({
	           type: "POST",
	           url: "policy/save_file_contents.php",
	           data:({'file':current_tab_title, 'content':newcontents,'filestats':'old'}),
	           //data: "name="+current_tab_title+"&content="+html_stripped,
	           success: function(data){
		           $confirmation.dialog({title: "Saved"});
	        	   $confirmation.html('<span>Operation completed sucessfully</span>'); 
	        	   $confirmation.dialog('open');
	        	   
	             },
            error:function(data){
	            	 $confirmation.dialog({title: "Error"});
	            	 $confirmation.html('<span>Error! occured'+data.msg+'</span>'); 
	            	 $confirmation.dialog('open');   
	             }
	       });
		 }
		 else
		 {
			 $("#tab_title").val(current_tab_title);
			 $("#tab_content").html(newcontents);
			 $("#event").val('saving');
			 $dialog.dialog('open');
		 }
	   });
	   
//The dialog to be shown to user action is complete i.e when ajax request completes.It may display both sucessful resut or unsucess result
 //This dilaogue uses div with id confirmation.
	 var $confirmation = $('#confirmation').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 resizable: false,
		 buttons: {
		 'Ok': function() {
	      $(this).dialog('close');
	      }
	 }
	 });
	 	
	 
	 var $chkcnf=$('#checkoutconfirmation').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 resizable: false,
		 title:'Checkout',
		 buttons: {
		 'Ok': function() {
			 $.ajax({
					   type: "POST",
					   url: "policy/empty_directory.php",
					   success: function(data){
						    $("#operation").val('checkout');
							$("#commentlbl").hide();
							$("#comments").hide();
							$("#datatype").val('json');
							$("#repo").show();
							$("#repolbl").show();
							$cfd.dialog('open');
							
						 },
					error:function(data){
							 $confirmation.dialog({title: "Error"});
							 $confirmation.html('<span>Error! occured'+data.msg+'</span>'); 
							 $confirmation.dialog('open');   
						 }
				   });
			 $(this).dialog('close');
	      },
		'Cancel':function() {
	      $(this).dialog('close');
	      }
	  }
	 });
	 
	  var $svnlogdlg = $('#svnlogtable').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 resizable: true,
		 height:500,
		 width:900,
		 title:'Change Logs',
		 buttons: {
		 'Ok': function() {
	      $(this).dialog('close');
	      }
	 }
	 });
	 
    var $sfd = $('#sfdialog').dialog({
		 autoOpen: false,
		 modal: true,
		 resizable: false,
		 hide: 'puff',
		 height: 'auto',
		 width:400,
		 closeText: 'hide',
		 buttons: {
		  'Save & Commit': function() {
			  var file=$('#tobesaved_name',this).val();
		    $.ajax({
	           type: "POST",
	           async:false,
	           url: "policy/save_file_contents.php",
	           data:({'file':$('#tobesaved_name',this).val(),'content':$('#tobesaved',this).val(),'filestats':'old'}),
	           
	           success: function(data){
				    $("#comments").show();
		            $("#commentlbl").show();
		            $("#repo").hide();
		            $("#repolbl").hide();
		            $("#operation").val('commit');
		            $("#datatype").val('json');
					$('#cmtfile').val(file);
					$cfd.dialog('open');
	             },
              error:function(data){
	            	 $confirmation.dialog({title: "Error"});
	            	 $confirmation.html('<span>Error! occured</span>'); 
	            	 $confirmation.dialog('open');   
	             }
	       }); 
		    $(this).dialog('close');
	       },
		 'Save': function() {
		 
		 $.ajax({
	           type: "POST",
	           async:false,
	           url: "policy/save_file_contents.php",
	           data:({'file':$('#tobesaved_name',this).val(),'content':$('#tobesaved',this).val(),'filestats':'old'}),
	           //data: "name="+current_tab_title+"&content="+html_stripped,
	           success: function(data){
                     $confirmation.dialog({title: "Saved"});
	        	   $confirmation.html('<span>Operation completed sucessfully</span>'); 
	        	   $confirmation.dialog('open');
	             },
              error:function(data){
	            	 $confirmation.dialog({title: "Error"});
	            	 $confirmation.html('<span>Error! occured</span>'); 
	            	 $confirmation.dialog('open');   
	             }
	       }); 
		 $(this).dialog('close');
	       },
		'Cancel': function() {
	 	   $(this).dialog('close');
		   if(closetabs)
		   {
			  $tabs.tabs('remove', closetabindex);
			  closetabindex="";
			  closetabs=false;
		   }
	 	   }
	 	 },
	 	open: function(event, ui) { 
	 	 	$(".ui-dialog-titlebar-close").hide();
	 	 }
	 });

	 $('#Checkout')
	   .click(function() {
			 if(code_editor_counter > 0)
		     {
				 alert("please close all the active policies tab");
				 
			 }
			 else
			 {
				  $.ajax({
	           type: "POST",
	           async:false,
	           url: "policy/get_list.php?op=ischeckedout",
	           success: function(data){
				   if(data=='1')
				     {
	            	 $chkcnf.html('<span>Working directory is already checkedout. Proceed and replace contents</span>'); 
	            	 $chkcnf.dialog('open');
					 }
					else
					{
					        $("#operation").val('checkout');
							$("#commentlbl").hide();
							$("#comments").hide();
							$("#datatype").val('json');
							$("#repo").show();
							$("#repolbl").show();
							$cfd.dialog('open');
					}
	             },
              error:function(data){
	            	 $confirmation.dialog({title: "Error"});
	            	 $confirmation.html('<span>Error! occured</span>'); 
	            	 $confirmation.dialog('open');   
	             }
	           });
			 }
         });

          $('#Commit')
	   .click(function() {
		 $("#comments").show();
		 $("#commentlbl").show();
		 $("#repo").hide();
		 $("#repolbl").hide();
		 $("#operation").val('commit');
		 $("#datatype").val('json');
		 $cfd.dialog('open');
          });

          $('#update')
	   .click(function() {
           $("#operation").val('update');
		   $("#commentlbl").hide();
		   $("#comments").hide();
		   $("#repo").hide();
		   $("#repolbl").hide();
		   $("#datatype").val('json');
		   if(code_editor_counter > 0)
		     {
				 alert("please close all the active policies tab");
			 }
			 else
			 {
				 $cfd.dialog('open');
			 }
           
         });
	   
	    $('#svnlogs')
	   .click(function() {
           $("#operation").val('svnlogs');
		   $("#commentlbl").hide();
		   $("#comments").hide();
		   $("#repo").show();
		   $("#repolbl").show();
		   $("#datatype").val('json');
           $cfd.dialog('open');
         });
       
	   $('#checksyntax')
	   .click(function() {
			$.ajax({
			type: "POST",
			async:false,
			url: "policy/checksyntax.php",
			dataType:'json',
			success: function(data){
				if(data.result=="SUCCESS")
				{
				$confirmation.dialog({title: "No Errors"});
				$confirmation.html('<span>Compiled Sucessfully with no errors</span>'); 
				$confirmation.dialog('open');
				}
				else
				{
				$confirmation.dialog({title: "Error Occured"});
				$confirmation.html('<span>'+data.result+'</span>'); 
				$confirmation.dialog('open');
				}
			},
		    error:function(data){
				$confirmation.dialog({title: "Error"});
				$confirmation.html('<span>Error! occured</span>'); 
				$confirmation.dialog('open');   
			}

		}); 	   
		});

          
   // dialogue to be displayed while making check out of the file

     var $cfd = $('#commitdlg').dialog({
		 autoOpen: false,
		 modal: true,
		 resizable: false,
		 hide: 'puff',
		 height: 'auto',
		 width:400,
		 closeText: 'hide',
		 buttons: {
		 'Ok': function() {
                        var keys;
			var passwd;
			$.jCryption.getKeys("policy/encrypt.php?generateKeypair=true",function(receivedKeys) {
				keys = receivedKeys;
				$.jCryption.encrypt($("#password").val(),keys,function(encrypted) {
					passwd=encrypted;
                        $.ajax({
						type: 'POST',
						url: "policy/svnmodule.php",
						data: {'passwd':passwd, 'op':$("#operation").val(),'file':$('#cmtfile').val(),'comments':$("#comments").val(),'user':$("#username").val(),'repo':$("#repo").val()},
                        dataType:$("#datatype").val(),
						success: function(data) {
							if($("#operation").val()=='checkout')
							  {
								  if(data.status)
								  {
								   var path = "policy/get_list.php";
									$("#container_policies_id").load(path,{dir: 'policies/'}, function(data){	
									});
								  //$('#Checkout').hide();
					$('<div id="repoText"><span>SVN Repository Used</span></div>').css({position: 'absolute' , width:'100%'}).hide().appendTo('body');
								  $('#usedRepo').html('<span>'+$("#repo").val()+'</span>');
								  $confirmation.dialog({title: $("#operation").val()});
								  $confirmation.html('<span>Checkedout sucessfully </span>'); 
								  }
								  else
								  {
								  $confirmation.dialog({title: $("#operation").val()});
								  $confirmation.html('<span>Check out failed </span>'); 
								  }
								  $confirmation.dialog('open');
								  
							  }
						  else if($("#operation").val()=='svnlogs')
							  { 
								$('#svnlogtable').html(data.table);
								$('#svnlogtable table').tableFilter();
                                $('#svnlogtable table').tablesorter({widgets: ['zebra']}); 
								/* $.fancybox({
								 //'orig' : $(this),
								 'padding' : 0,
								 'href' : '#svnlogtable', 'title' : 'ChangeLogs',
								 'transitionIn' : 'elastic',
								 'transitionOut' : 'elastic'
								});*/ 
								$svnlogdlg.dialog('open');
							  }
						else if($("#operation").val()=='update')
							{
								var path = "policy/get_list.php";
							    $("#container_policies_id").load(path,{dir: 'policies/'}, function(data){});
								 $confirmation.dialog({title: $('#cmtfile').val()});
								 $confirmation.html('<span>Updated Sucessfully </span>'); 
								 $confirmation.dialog('open');
							}
						else if($("#operation").val()=='commit')
							{
								 $confirmation.dialog({title: $("#operation").val()});
								 $confirmation.html('<span>'+$('#cmtfile').val()+' commited Sucessfully </span>'); 
								 $confirmation.dialog('open');
							}
							$('#cmtfile').val('');
						},
					error:function(data){
						$confirmation.dialog({title: "Error"});
						$confirmation.html('<span>Cannot Process the request '+data.status+'</span>');
						$confirmation.dialog('open');
						   $('#cmtfile').val('')
						}
					});
				   });
			});
			$(this).dialog('close');
	       },
		'Cancel': function() {
	 	   $(this).dialog('close');
		   $('#cmtfile').val('');
	 	   }
	 	 },
	 	open: function(event, ui) {
		$(this).dialog({title: $("#operation").val()});
		 $("#password").val('');
		 $("#comments").val('');
		 $("#username").val('');
         $("#repo").val('');
	 	 },
		 close: function(event, ui){ 
		 }
	 });

      /*   var $svndlg = $('#svnauth').dialog({
		 autoOpen: false,
		 modal: true,
		 resizable: false,
		 hide: 'puff',
		 height: 'auto',
		 width:400,
		 closeText: 'hide',
                 buttons: {
		 'Ok': function() {
                        var keys;
			var passwd;
			$.jCryption.getKeys("policy/encrypt.php?generateKeypair=true",function(receivedKeys) {
				keys = receivedKeys;
				$.jCryption.encrypt($("#svnauthpassword").val(),keys,function(encrypted) {
					passwd=encrypted;
                                   $.ajax({
						type: "POST",
						async:false,
						url: "policy/svnmodule.php",
						data:({'op':$("#operation").val(),'user':$("#user").val(),'passwd':passwd,'file':current_tab_title}),
						dataType:'json',
						//data: "name="+current_tab_title+"&content="+html_stripped,
						success: function(ret){
						    //alert(ret.status);
						 if(ret.status)
						  {
							var path = "policy/get_list.php";
							$("#container_policies_id").load(path,{dir: 'policies/'}, function(data){	
							});
							$confirmation.dialog({title: $("#operation").val()});
							$confirmation.html('<span>Operation completed sucessfully</span>'); 
							$confirmation.dialog('open');
							$("#user").val('');
							$("#svnauthpassword").val('');
						  }
						},
				
					error:function(data){
						$confirmation.dialog({title: "Error"});
						$confirmation.html('<span>Error! occured</span>');
						$confirmation.dialog('open');
						}
	                            }); 
                                });
                            });
			    $(this).dialog('close');
                },
		'Cancel': function() {
	 	   $(this).dialog('close');
	 	   }
	 	 },
	 	open: function(event, ui) {	
	 	 }

	 });*/
         
    /* $('#update')
		.click(function() {
		$.ajax({
			type: "POST",
			async:false,
			url: "policy/svnmodule.php",
			data:({'op':'update','file':current_tab_title,'user':$("#user").val(),'passwd':passwd}),
			//data: "name="+current_tab_title+"&content="+html_stripped,
			success: function(data){
				var path = "policy/get_list.php";
				$("#container_policies_id").load(path,{dir: 'policies/'}, function(data){	
				});
				$confirmation.dialog({title: "Updated out"});
				$confirmation.html('<span>Operation completed sucessfully</span>'); 
				$confirmation.dialog('open');

			},

		error:function(data){
				$confirmation.dialog({title: "Error"});
				$confirmation.html('<span>Error! occured</span>'); 
				$confirmation.dialog('open');   

			}

		}); 

	});*/
	$('#repoText').live('click',function(){
		$('#usedRepo').css({top: '0' , left:'0', margin:'17px' , opacity:'0.9'});
		$('#usedRepo').slideToggle("slow");							 
	 });
	
	jQuery(document).ajaxStart(function(){
		 $('#spinner').css({top: '0' , left:$('body').width()/2 }).fadeIn();
		 $('#repoText').css({top: '0' , left:'0'}).fadeOut();
		 });
	 jQuery(document).ajaxStop(function(){
		 $('#spinner').css({top: '0' , left:$('body').width()/2 }).fadeOut();
		 if(closetabs)
		   {
			  $tabs.tabs('remove', closetabindex);
			  closetabindex="";
			  closetabs=false;
		   }
		 $('#repoText').css({top: '0' , left:'0' }).fadeIn();
		 });
	   
						   
});
 </script>
</body>
</html>
