<div id="editor">

	<div id="leftPanel">
	 	<div class="ui-layout-content">
		  <div id="Policies">
		    <div class="title">Company Nova Policies</div>
		    <ul id="policies_list">
		    
		    </ul>
		  </div>
		  
		  <div id="help-panel">
		  <div class="title">Editing help-Copernicus</div>
		  <div class="results">
		  
		  </div>
		  <div class="links">
			  <span>Examples</span>
			  <span>Reference Manual</span>
			  <span>Commands</span>
			  <span>Best Practices</span>
		  </div>
		  <div class="shortcuts">
			  <span>Shortcuts:</span>
			  <span>Ctrl+space-&gt; synatx autocompletion</span>
			  <span>Tab       -&gt; indent</span>
			  <span>Ctrl+z    -&gt; undo</span>
			  <span>Ctrl+y    -&gt; Redo</span>
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
				<input type="text" class="ui-widget-content ui-corner-all" value="" id="tab_title" name="tab_title">
			
				<textarea class="ui-widget-content ui-corner-all" id="tab_content" name="tab_content"></textarea>
			</fieldset>
		</form>
	</div>
	
<div title="confirmation" id="confirmation">	 
</div>

<div title="Save File" id="sfdialog">
<span></span>
<input type="hidden" id="tobesaved" ></input>
<input type="hidden" id="tobesaved_id"></input>
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
		   
		   paneSelector:         "#header"
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
	 var policy_id=$("input[name='id']",tab_id).val();
	 var tabtype=$("input[name='tabtype']",tab_id).val();

     var tab_title=$('a',$(this).parent()).html();
	 if(tabtype=='codeEditor')//if the tab is of type editor and it should complete couple of things befor closing.
	 {
		 var tab_contents=$('iframe',tab_id).contents().find('.editbox').html();
		 var presrevelinespace=tab_contents.replace(/<br>/g,"\\n");
		 var newcontents = presrevelinespace.replace(/(<.*?>)/ig,"");
		 if(policy_id!=undefined)//tab is old so update necessary things
		 {
			 $.ajax({
			        type: "POST",
			        url: "<?php echo site_url('policy/get_policy')?>/"+policy_id,
			        dataType:'json',
			        global: false,
			        success: function(data){
				        var oldcontents=data.content;
				        if(oldcontents!=newcontents)
				        { 
					       //alert(oldcontents+'     '+newcontents);
					       $('#tobesaved_id',$sfd).val(policy_id);
					       $('#tobesaved',$sfd).val(newcontents);
					       $('span',$sfd).html('The contents of the '+tab_title+' have been modified. Do you want to save the changes?');
				           $sfd.dialog('open');
				           
				        }
				        
			        }
			 });
			 $('#recordsArray_'+policy_id).show('fast');
		 }
		 else//tab is new so dispay dialog before closing
		 {
			 $("#tab_title").val(tab_title);
			 $("#tab_content").html(newcontents);
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
           url: "<?php echo site_url('policy/add') ?>/",
           data:({'name':$('#tab_title').val(), 'content':$('#tab_content').html()}),
           dataType:'json',
           success: function(data){
               
               var append_html='<li><span>'+data.title+'</span><a href="#" id="recordsArray_'+data.id+'">edit</a><li>';
               var append_html_tab='<input type="hidden" name="id" value="'+data.id+'" />';
               $('#policies_list').append(append_html);
               $(current_tab_id).append(append_html_tab);
               $('a[href="'+current_tab_id+'"]',$tabs).html(data.title);
               $('#recordsArray_'+data.id).hide('slow');
               
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
		var tab_contents=$('iframe',current_tab_id).contents().find('.editbox').html();
		var presrevelinespace=tab_contents.replace(/<br>/g,"\\n");
		 var html_stripped = presrevelinespace.replace(/(<.*?>)/ig,"");
		 $("#tab_title").val(current_tab_title);
		 $("#tab_content").html(html_stripped);
		 var policy_id=$("input[name='id']",current_tab_id).val();
		 if(policy_id==undefined)
		 {
			 $dialog.dialog('open');
		 }
		 else
		 {
			 $.ajax({
		           type: "POST",
		           url: "<?php echo site_url('policy/update')?>/"+policy_id,
		           data:({'name':current_tab_title, 'content':html_stripped}),
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
		 }
	 });

//load policies from database into left hand side panel on list with id Policies_list
	var path = "<?php echo site_url('policy/get_list') ?>/";
	$("ul#policies_list").load(path);

 //hooking up the link clicked in the policies list displayed in left hand side for editing purpose.
	 $('ul#policies_list li a').live('click', function() {
		 var fullID = $(this).attr("id");
		 var pid=fullID.substr(13);
		 $(this).hide('slow');
		 $.ajax({
		        type: "POST",
		        url: "<?php echo site_url('policy/get_policy') ?>/"+pid,
		        dataType:'json',
		        success: function(data){
		        	tab_title_input=data.title;
		        	tab_content_input='<input type="hidden" name="id" value="'+pid+'" /><input type="hidden" name="tabtype" value="codeEditor" />'+
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
		          }
		       }); 
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
		var error=$('span.cf-error',container).html();
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
		
		if(error==null)//if there is syntax error in the editor indicated by the css of the editor
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
	           url: "<?php echo site_url('policy/update')?>/"+$('#tobesaved_id',$sfd).val(),
	           data:({'name':current_tab_title,'content':$('#tobesaved',this).val()}),
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






































































