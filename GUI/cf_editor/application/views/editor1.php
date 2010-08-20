<div id="editor">

	<div id="leftPanel">
	 	<div class="ui-layout-content">
		  <div id="Policies">
		    <div class="title">Company Nova Policies</div>
		    <ul id="policies_list">
		    
		    </ul>
		  </div>
		  
		  <div id="help-panel">
		  
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
<script type="text/javascript">

$(document).ready(function() {
	//setting up collapseable splitter layout
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
	   ,  size:                  "auto"
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
	//alert("selected="+ui.index);
	
    },
    fx: { 
        opacity: 'toggle',speed:'fast'
        },
    remove: function(event, ui) {
        	//alert($tabs.tabs( "length"));
        	current_tab_index=current_tab_index-1;
        	//alert("from remove"+current_tab_index);
    //$tabs.tabs('select',$tabs.tabs( "length")-1);
        	tab_counter--;
         }
     }); 

   //consume the full space of right panel by tabs on document load
   var tab_height=$('div#rightPanel').height();
	$("#tabs").css('height', tab_height-15 );
	$(".ui-tabs-panel").css('height', tab_height-50 );
	$('<img src="<?php echo base_url();?>application/assets/Images/ajax-loader.gif" id="spinner" />').css('position','absolute').hide().appendTo('body');
	
	
	//tab adding function
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

	//adding the editor in newly created tab.
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
		 //$('div.CodeMirror-wrapping',current_tab_id).css('height',$(".ui-tabs-panel").height()-20);
		// alert($(".CodeMirror-wrapping").attr('style'));
		$('.CodeMirror-wrapping',current_tab_id).get(0).style.height=$(current_tab_id).get(0).style.height;
		 // alert($('.CodeMirror-wrapping',current_tab_id).get(0).style.height);
    	 
	 }); 
	 
//respond to the tab closing event/removing event	  
	$('#tabs span.ui-icon-close').live('click', function() {
	 var index = $('li',$tabs).index($(this).parent());
	 var tab_id=$('a',$(this).parent()).attr("href");
	 var policy_id=$("input[name='id']",tab_id).val();
	 var tabtype=$("input[name='tabtype']",tab_id).val();

     var tab_title=$('a',$(this).parent()).html();
	 var tab_contents=$('iframe',tab_id).contents().find('.editbox').html();
	 var presrevelinespace=tab_contents.replace(/<br>/g,"\\n");
	 var newcontents = presrevelinespace.replace(/(<.*?>)/ig,"");
	 if(tabtype=='codeEditor')
	 {
		 if(policy_id!=undefined)
		 {
			 $.ajax({
			        type: "POST",
			        url: "<?php echo site_url('policy/get_policy') ?>/"+policy_id,
			        dataType:'json',
			        global: false,
			        success: function(data){
				        var oldcontents=data.content;
				        if(oldcontents!=newcontents)
				        { 
					       $('#tobesaved_id',$sfd).val(policy_id);
					       $('#tobesaved',$sfd).val(newcontents);
					       $('span',$sfd).html('The contents of the '+tab_title+' have been modified. Do you want to save the changes?');
				           $sfd.dialog('open');
				           
				        }
				        
			        }
			 });
			 $('#recordsArray_'+policy_id).show('fast');
		 }
		 else
		 {
			 $("#tab_title").val(tab_title);
			 $("#tab_content").html(newcontents);
			 $dialog.dialog('open');
		 }
		 code_editor_counter--;
	 }
	 $tabs.tabs('remove', index);
	 });

	 
//open a dialogue before commiting the file
	var $dialog = $('#dialog').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 buttons: {
		 'Save': function() {
		 //addTab();//save the contents of tab in file

		 $.ajax({
           type: "POST",
           url: "<?php echo site_url('policy/add') ?>/",
           //data: "name="+$('#tab_title').val()+"&content="+$('#tab_content').html(),
           data:({'name':$('#tab_title').val(), 'content':$('#tab_content').html()}),
           dataType:'json',
           success: function(data){
               //$('#policies_list').html(data);
              
               var append_html='<li><span>'+data.title+'</span><a href="#" id="recordsArray_'+data.id+'">edit</a><li>';
               var append_html_tab='<input type="hidden" name="id" value="'+data.id+'" />';
               $('#policies_list').append(append_html);
               $(current_tab_id).append(append_html_tab);
               $('a[href="'+current_tab_id+'"]',$tabs).html(data.title);
               $('#recordsArray_'+data.id).hide('slow');
               //alert(data.html);
               
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
		 $("#tab_content").hide();
		 },
		 close: function() {
		 //$form[0].reset();
		 }
		 });
 
//perform selection of tab and contents before saving and decide the editing tab contains new/old item
	$('#save')
	 .click(function() {
		 //alert(current_tab_id);
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
			 //alert(tab_contents);
			 //return;
			 $.ajax({
		           type: "POST",
		           url: "<?php echo site_url('policy/update')?>/"+policy_id,
		           data:({'name':current_tab_title, 'content':html_stripped}),
		           //data: "name="+current_tab_title+"&content="+html_stripped,
		           success: function(data){

		        	   $confirmation.html('<span>Operation completed sucessfully</span>'); 
		        	   $confirmation.dialog('open');
		        	   
		             },
                  error:function(data){
		            	 $confirmation.html('<span>Error! occured</span>'); 
		            	 $confirmation.dialog('open');   
		             }
		       }); 
		 }
	 });

//load policies from database
	var path = "<?php echo site_url('policy/get_list') ?>/";
	$("ul#policies_list").load(path);

 //edit file inlist
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
		        	                  
                    
		       	    addTab('existing');
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
		          }
		       }); 
	 });

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
		
		if(error==null)
		{
			$.ajax({
		           type: "POST",
		           url: "<?php echo site_url('policy/check_syntax')?>/",
		           data:({'name':current_tab_title, 'content':tab_contents}),
		           dataType:'json',
		           success: function(data){
			           if(data.result=='successfull')
			           {
			        	   $confirmation.html('<span>'+data.details+' for policy '+data.policy_name+'</span>'); 
			        	   $confirmation.dialog('open');
			           }
		        	   
		             },
	            error:function(data){
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
		  var html="<span>syntax error in line no:"+error+"</span>";
		  $confirmation.html(html); 
   	      $confirmation.dialog('open');
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
		 'Save': function() {
		 $.ajax({
	           type: "POST",
	           async:false,
	           url: "<?php echo site_url('policy/update')?>/"+$('#tobesaved_id',$sfd).val(),
	           data:({'name':current_tab_title,'content':$('#tobesaved',this).val()}),
	           //data: "name="+current_tab_title+"&content="+html_stripped,
	           success: function(data){

	        	   $confirmation.html('<span>Operation completed sucessfully</span>'); 
	        	   $confirmation.dialog('open');
	        	   
	             },
              error:function(data){
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

	 jQuery(document).ajaxStart(function(){
		 $('#spinner').css({top: '0' , left:$('body').width()/2 }).fadeIn();
		 });
	 jQuery(document).ajaxStop(function(){
		 $('#spinner').css({top: '0' , left:$('body').width()/2 }).fadeOut();
		 });	 	 
	 
 });

</script>






































































