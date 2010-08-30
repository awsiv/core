<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
		<title>CFengine Online Editior</title>
		<link rel="stylesheet" type="text/css" href="<?php echo base_url();?>application/assets/css/jquery-ui-1.8.2.custom.css" />
		<link type="text/css" rel="stylesheet" href="<?php echo base_url();?>application/assets/css/jqueryFileTree.css" />
		<link type="text/css" rel="stylesheet" href="<?php echo base_url();?>application/assets/css/nova.css" />
		<script src="<?php echo base_url();?>application/assets/scripts/combo.js" type="text/javascript"></script>
		<script src="<?php echo base_url();?>application/assets/scripts/jquery-1.4.2.min.js" type="text/javascript"></script>
		<script src="<?php echo base_url();?>application/assets/scripts/jquery.layout.min-1.2.0.js" type="text/javascript"></script>
		<script src="<?php echo base_url();?>application/assets/scripts/jquery-ui-1.8.2.custom.min.js" type="text/javascript"></script>
		<script src="<?php echo base_url();?>application/assets/scripts/jquery.contextmenu.src.js" type="text/javascript"></script>
		<script src="<?php echo base_url();?>application/assets/scripts/codemirror.js" type="text/javascript"></script>
        <script src="<?php echo base_url();?>application/assets/scripts/cf.js" type="text/javascript"></script>
        <script src="<?php echo base_url();?>application/assets/scripts/jqueryFileTree.js" type="text/javascript"></script>
		<style type="text/css">
      .completions {
        border: 1px solid black;
        z-index: 10;
        overflow: hidden;
      }
      .completions select {
        outline: none;
        padding: 0;
        margin: 0;
        border: 0;
        background-color: white;
        font-family: monospace;
        font-size: 80%;
      }
      .CodeMirror-wrapping {
        border-top: 1px solid #911;
        border-bottom: 1px solid #911;
        padding: 3px;
      }
      .CodeMirror-line-numbers {
        margin: 0;
        padding: 0;
        padding-top: 3px;
        margin-right: .4em;
        width: 2.2em;
        color: #aaa;
        text-align: right;
        padding-right: .3em;
        font-size: 10pt;
        font-family: monospace;
      }
    </style>
		
    </head>
    <body>
	     <div id="top">
	       <form action="http://demo.cdengine.com/knowledge.php" method="post">
		       <p>
		       <input class="searchfield" type="text" name="search_string" />&nbsp;Search
		       </p>
	       </form>
	       <h2>
	       CFENGINE MISSION PORTAL
	       <span id="subheading"><b>overview</b></span>
	       </h2>
	       <br class="clear_right" />
	     </div>
	     
	<div id="editor">

	<div id="leftPanel">
	 	<div class="ui-layout-content">
		  <div id="menu">
			  <a href="#" id="new">New</a>
			  <a href="#" id="compile">Check syntax</a>
			  <a href="#" id="save">Save</a>
			  <a href="#" id="commit">Commit</a>
			  <a href="#" id="update">Update</a>
			  <div class="shortcuts">
				  <span>Shortcuts:</span>
				  <span>Ctrl+space-&gt; synatx autocompletion</span>
				  <span>Tab       -&gt; indent</span>
				  <span>Ctrl+z    -&gt; undo</span>
				  <span>Ctrl+y    -&gt; Redo</span>
		     </div>
		  </div>
		  
		  <div id="Policies">
		  <div class="title">Company Nova Policies</div>
		    <div id="container_policies_id">
		    
		    </div>
		  </div>
		</div>
	</div>
	
	<div id="rightPanel">
	<div class="ui-layout-content">
	 <div id="tabs">
		  <ul>
			   <li>
			   <a href="#news">News From CFengine</a>
			   <!--<span class="ui-icon ui-icon-close">Remove Tab</span>-->
			   </li>
		  </ul>
		  <div id="news">
		  Proin elit arcu, rutrum commodo, vehicula tempus, commodo a, risus. 
		  Curabitur nec arcu. Donec sollicitudin mi sit amet mauris. Nam elementum quam ullamcorper ante.
		  Etiam aliquet massa et lorem. Mauris dapibus lacus auctor risus. Aenean tempor ullamcorper leo.
		   Vivamus sed magna quis ligula eleifend adipiscing. Duis orci. Aliquam sodales tortor vitae ipsum. Aliquam nulla. Duis aliquam molestie erat. Ut et mauris vel pede varius sollicitudin. Sed ut dolor nec orci tincidunt interdum. 
		  Phasellus ipsum. Nunc tristique tempus lectus.
		  </div>
	 </div>
	 
	<!--  <div id="syntax_check_area">Area for checking syntax</div>-->
	
	</div>
</div>
</div>
<div title="Tab data" id="dialog">
		<form>
			<fieldset class="ui-helper-reset">
				<label for="tab_title">File Name</label>
				<input type="text" class="ui-widget-content ui-corner-all" value="" id="tab_title" name="tab_title" />
			
				<textarea class="ui-widget-content ui-corner-all" id="tab_content" name="tab_content"></textarea>
				<input type="hidden" id="event"></input>
			</fieldset>
		</form>
	</div>
	
<div title="confirmation" id="confirmation">	 
</div>

<div title="Save File" id="sfdialog">
<span></span>
<input type="hidden" id="tobesaved" ></input>
<input type="hidden" id="tobesaved_name"></input>
</div>
 <div class="contextMenu" id="myMenu1">
      <ul>
        <li id="lookup">Look up in Copernicus</li>
        <li id="correct">Correct it</li>
      </ul>
    </div>
    
    <script type="text/javascript">

$(document).ready(function() {
	//setting up collapseable splitter layout with for options north is towards the top and south is towards the 
	//the bottom and is west is at the left hand side.
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
		   
		   paneSelector:         "#top"
	   ,  size:                  75
	   ,  spacing_open:          0
	   ,  resizable:             false
	   ,  closeable:             false
	      
	   }
	,  south: {
		paneSelector:         "#footer"
	   ,  spacing_open:          0
	   ,  resizable:             false
	   ,  closeable:             false
	   }
	});

	//initialization and declaration of necessary variables
	var tab_counter = 2;
	var no_seachlink=1;
	var code_editor_counter=1;
	var tab_title_input="Untitled-";
	var tab_content_input="";
	var current_tab_content="";
	var current_tab_title="";
	var current_tab_index="";
	var current_tab_id="tabs-1";
	var cur_file_path="";
	 $title_element="";


   //initializing the dom element to behave as tab using jquery along with different events attached to it 
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

   //consume the full space of right panel by tabs on document load
   var tab_height=$('div#rightPanel').height();
	$("#tabs").css('height', tab_height-15 );
	$(".ui-tabs-panel").css('height', tab_height-50 );
	
	//adding and hiding ajax process gif file in the dom.
	$('<img src="<?php echo base_url();?>application/assets/Images/ajax-loader.gif" id="spinner" />').css('position','absolute').hide().appendTo('body');
	
	
	//tab adding function and deciding where to put untitle title on tab..
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

  // Function for adding context menu when a new tab is added with the editor in it.
  function addcontextmenu()
  {
	  $('iframe',current_tab_id).bind('load',function(){
		  var iframe=this;
		  $(this).contents().find('.editbox').bind('mouseup',function(event){
			  var quest = 'sad';
			  var win=iframe.contentWindow;//for firefox
			  var doc=win.document;        //for ie
			  if (win.getSelection) {
				    quest= win.getSelection().toString();
				  } else if (doc.selection && doc.selection.createRange) {
				     quest=doc.selection.createRange().text;
				  }
			  if(event.which==1)
			  {
				  $('#jqContextMenu').hide();
			  } 
			  if(event.which==3)
			  {
				  $(this).contextMenu('myMenu1',{
					  shadow:false,
					  eventPosX:Math.floor($('.CodeMirror-wrapping',current_tab_id).offset().left),
					  eventPosY:Math.floor($('.CodeMirror-wrapping',current_tab_id).offset().top),
					  bindings: {
				        'lookup': function(t) {
				         if(quest!="")
				         {
				          var html='<span><a href="http://www.cfengine.com/?search='+quest+'">?'+quest+'</a></span>';
				          if(no_seachlink<=4)
				          {
				          $('div.results','#help-panel').append(html);
				          no_seachlink++;
				          }
				          else
				          {
				          $('div.results','#help-panel').html(html);
				          no_seachlink=0;
				          }
				         }
				        },
				        'correct': function(t) {
				          alert('Trigger was '+t.id+'\nAction was correct');
				        }
				      }		    
					  });
			  }
			 });
		}).each(function(){
			
			$(this).trigger('load');//for google chrome as load happenly only once in chrome
			//if(this.complete || (jQuery.browser.msie && parseInt(jQuery.browser.version) == 6))
				//alert(jQuery.browser);
				});
  }

	//hooking up the new link click event for adding the editor in newly created tab as well as adding context menu to it.
	$('#new')
     .click(function() {
    	 tab_content_input='<textarea id="code'+code_editor_counter+'" cols="120" rows="30">body monitor control\n{\nforgetrate => "0.7";\n}</textarea>'+
    	                  '<input type="hidden" name="tabtype" value="codeEditor" />';              
    	 addTab('new');
    	 var editor = CodeMirror.fromTextArea('code'+code_editor_counter, {
    		    parserfile: ["cfsyntax.js", "parsecf.js"],
    		    stylesheet: "<?php echo base_url();?>application/assets/css/cfcolors.css",
    		    path: "<?php echo base_url();?>application/assets/scripts/",
    		    lineNumbers: true,
    		    styleNumbers: styleLineNum,
    		    initCallback: function(e){initComplete(e); initResize(e); initChangeTracker(e);},
    		    onChange: contentChanged,
    		    continuousScanning: 500
    		  });
		  
		code_editor_counter++; 
		$('.CodeMirror-wrapping',current_tab_id).get(0).style.height=$(current_tab_id).get(0).style.height;
		addcontextmenu();
		    });

	
	 
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
			 presrevelinespace=tab_contents.replace(/<br(?:"[^"]*"['"]*|'[^']*'['"]*|[^'">])+>/gi,"\\n");
			 newcontents = presrevelinespace.replace(/<(?:"[^"]*"['"]*|'[^']*'['"]*|[^'">])+>/ig,"");
		 }
		 else
		 {
			 presrevelinespace=tab_contents.replace(/<br[^>]*>/gi,"\\n");
			 newcontents = presrevelinespace.replace(/(<.*?>)/ig,"");
		 }
		 if(file_type!=undefined)//tab is old so update necessary things
		 {
			 $.ajax({
			        type: "POST",
			        url: "<?php echo site_url('policy/compare_file_contents')?>/",
			        data:({'file_path':'jqueryFileTree/'+tab_title,'newcontents':newcontents,'agent':agent}),
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
	 $tabs.tabs('remove', index);
	 });

	 
//Dialog to be displayed before closing a newly created tab with editor in it.This dialog uses the  div with id dialog for ui.
	var $dialog = $('#dialog').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 buttons: {
		 'Save': function() {
		 $.ajax({
           type: "POST",
           url: "<?php echo site_url('policy/save_file_contents') ?>/",
           data:({'file':$('#tab_title').val(), 'content':$('#tab_content').html()}),
           dataType:'json',
           success: function(data){
        	   var id= $('ul#policies_list_new li').length+1;
               var append_html='<li class="file ext_txt"><a href="#" rel="'+data.path+'" id="policy_'+id+'">'+data.title+'</a></li>';
               $('#policies_list_new').append(append_html);	
               if($('#event').val()=='saving')
	               {
	  			   var append_html_tab='<input type="hidden" name="link" value="policy_'+id+'" />';
	  			   $(current_tab_id).append(append_html_tab);
	               $('a[href="'+current_tab_id+'"]',$tabs).html(data.title);
	               $('#policy_'+id).hide('slow');
	               }
             }
          });
		 $(this).dialog('close');
		 },
		 'Cancel': function() {
		 $(this).dialog('close');
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
			 presrevelinespace=tab_contents.replace(/<br(?:"[^"]*"['"]*|'[^']*'['"]*|[^'">])+>/gi,"\\n");
			 newcontents = presrevelinespace.replace(/<(?:"[^"]*"['"]*|'[^']*'['"]*|[^'">])+>/ig,"");
		 }
		 else
		 {
			 presrevelinespace=tab_contents.replace(/<br[^>]*>/gi,"\\n");
			 newcontents = presrevelinespace.replace(/(<.*?>)/ig,"");
		 }

		 if(file_type!=undefined)//decide if its not the new file
		 {
		 $.ajax({
	           type: "POST",
	           url: "<?php echo site_url('policy/save_file_contents')?>/",
	           data:({'file':current_tab_title, 'content':newcontents}),
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

//load policies from database into left hand side panel on list with id Policies_list
	var path = "<?php echo site_url('policy/get_list') ?>/";
	$("#container_policies_id").load(path,{dir: 'jqueryFileTree/'}, function(data){
		
	});

	$('ul#policies_list_new li a').live('click',function(){
        openfile($(this).attr('rel'),$(this).attr('id'));
        $('#'+$(this).attr('id')).hide('slow');
	});
	 /*$('#container_policies_id').fileTree({
	        root: 'jqueryFileTree/',
	        script: path,
	        expandSpeed: 1000,
	        collapseSpeed: 1000,
	        multiFolder: false
	    }, function(file) {
	        //alert(file.split('*')[1]);
	       openfile(file.split('*')[0],file.split('*')[1]);
	    });*/

	

	//hooking up the link clicked in the policies list displayed in left hand side for editing purpose.    
	 function openfile(file,link_id)
	 {
		 $.ajax({
	           type: "POST",
	           url: "<?php echo site_url('policy/get_file_contents')?>/",
	           data:({'file_path':file}),
	           dataType:'json',
	           success: function(data){
	        	   tab_title_input=data.filename;
	        	   cur_file_path=data.information.server_path;
		           tab_content_input='<input type="hidden" name="tabtype" value="codeEditor" /><input type="hidden" name="link" value="'+link_id+'" />'+
		                              '<textarea id="code'+code_editor_counter+'" cols="120" rows="30">'+data.content.replace(/\\n/g,"\n")+'</textarea>';
		                              
		       	    addTab('existing');//adding the new tab
			       	 var editor = CodeMirror.fromTextArea('code'+code_editor_counter, {
			       		    parserfile: ["cfsyntax.js", "parsecf.js"],
			       		    stylesheet: "<?php echo base_url();?>application/assets/css/cfcolors.css", // you may have to adjust the path
			       		    path: "<?php echo base_url();?>application/assets/scripts/",
			       		    lineNumbers: true,
			       		    styleNumbers: styleLineNum,
			       		    initCallback: function(e){initComplete(e); initResize(e); initChangeTracker(e);},
			       		    onChange: contentChanged,
			       		    continuousScanning: 500
			       		  });
		   		code_editor_counter++;
		   		$('.CodeMirror-wrapping',current_tab_id).get(0).style.height=$(current_tab_id).get(0).style.height;
		   		addcontextmenu();//adding the context menu to the tab newly added
	             },
          error:function(data){
	            	 $confirmation.dialog({title: "Error"});
	            	 $confirmation.html('<span>Error! while performing request</span>'); 
	            	 $confirmation.dialog('open');   
	             }
	       }); 
	 }
		
	
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

 //hook up Check syntax link clicked event to perform syntax checking on both client side and on the server side.
 //it looks for syntax error in the client side first.If no syntax error exists on client side, the server is now allowed 
 //to process and ultimately the result is supplied to the user.
	 $('#compile').click(function(){
		 var tabtype=$("input[name='tabtype']",current_tab_id).val();
		 if(tabtype==undefined)
		 {
			 alert("Sorry! the contents cannot be compiled");
			 return;
		 }
		var container=$('iframe',current_tab_id).contents().find('.editbox');
		var tab_contents=container.text();
		var error=$('span.cf-error',container).html().trim();
		var linenumber=new Array();
		var errorcount=0;
		var linecontainer=$('div.CodeMirror-wrapping',current_tab_id).contents().find('.CodeMirror-line-numbers');
		$('div',linecontainer).each(function(){
			if($(this).attr('style')!=undefined)
			{
				linenumber[errorcount]=$(this).html();
				errorcount++;
				
			}
		});
		if(error==null ||error=='')//if there is syntax error in the editor indicated by the css of the editor
		{
			$.ajax({
		           type: "POST",
		           url: "<?php echo site_url('policy/check_syntax')?>/",
		           data:({'name':current_tab_title, 'content':tab_contents}),
		           dataType:'json',
		           success: function(data){
			           if(data.result=='successfull')
			           {
			        	   $confirmation.dialog({title: "Syntax valid"});
			        	   $confirmation.html("<span> Syntax for policy "+data.policy_name+" is valid</span>"); 
			        	   $confirmation.dialog('open');
			           }
			           else
			           {
			        	   $confirmation.dialog({title: "Syntax invalid"});
			        	   $confirmation.html("<span> Syntax for policy "+data.policy_name+" is invalid :"+data.details+"</span>"); 
			        	   $confirmation.dialog('open');
			           }
		        	   
		             },
	            error:function(data){
		            	 $confirmation.dialog({title: "Error"});
		            	 $confirmation.html('<span>Error! while performing request</span>'); 
		            	 $confirmation.dialog('open');   
		             }
		       });
		}
		else 
		{
		  var error="";
		  var i=0;
		  for(i=0 ;i<=linenumber.length-1 ; i++)
		  {
			  if(i==linenumber.length-1 && i!=0)
			  {
			      error+=" and "+linenumber[i];
			  }
			  else if(linenumber.length==1)
			  {
				  error+=linenumber[i];
			  }
			  else
			  {
			      error+=linenumber[i]+", ";  
			  }
		  }
		  $confirmation.dialog({title: "Syntax invalid"});
		  var html="<span>Syntax for policy "+current_tab_title+" is invalid on line:"+error+"</span>";
		  $confirmation.html(html); 
   	      $confirmation.dialog('open');
		}
			
		 
	 });

 //Save File Dialog to be displayed when user closed the tab that contains the editor.It prompts user to save the file,and when decided it saves the file.
	 var $sfd = $('#sfdialog').dialog({
		 autoOpen: false,
		 modal: true,
		 resizable: false,
		 hide: 'puff',
		 height: 'auto',
		 width:400,
		 closeText: 'hide',
		 buttons: {
		 'Save': function() {
		 
		 $.ajax({
	           type: "POST",
	           async:false,
	           url: "<?php echo site_url('policy/save_file_contents')?>/",
	           data:({'file':$('#tobesaved_name',this).val(),'content':$('#tobesaved',this).val()}),
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
	 	   }
	 	 },
	 	open: function(event, ui) { 
	 	 	$(".ui-dialog-titlebar-close").hide();
	 	 }
	 });

// hooking up the links generated by the conext menu in editor for helping purpose. It listens to any click event of those links and injects and iframe 
//inside a newly created a tab where the help page related to it. It has tobe  done as cross domain ajax posting is not possible, if help page 
//is in the same domain, simple ajax request can load a page in the desired div.
	 $('a','div.results').live('click',function(event){
	  event.preventDefault();
	  var url=$(this).attr('href');
	  tab_title_input=$(this).html();;
	  tab_content_input='<iframe class="searchframe"></iframe>';
       addTab('existing');
      $('iframe',current_tab_id).attr('src', url);
	 });

//showing and hiding the progress gif on ajax request start and stop event respectively.
	 jQuery(document).ajaxStart(function(){
		 $('#spinner').css({top: '0' , left:$('body').width()/2 }).fadeIn();
		 });
	 jQuery(document).ajaxStop(function(){
		 $('#spinner').css({top: '0' , left:$('body').width()/2 }).fadeOut();
		 });	

			 	 
	 
 });
</script>
    </body>

    </html>