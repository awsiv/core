      
	 /*  var tab_counter = 2;
		var code_editor_counter=1;
		var tab_title_input="Untitled-";
		var tab_content_input="";
		var current_tab_content="";
		var current_tab_title="";
		var current_tab_index="";
		var current_tab_id="tabs-1";
		var cur_file_path="";
		$title_element="";
		
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
	   
	   var tab_height=$('div#rightPanel').height();
	   $("#tabs").css('height', tab_height-15 );
	   $(".ui-tabs-panel").css('height', tab_height-50 );
	   
	   $('<img src="images/ajax-loader.gif" id="spinner" />').css('position','absolute').hide().appendTo('body');
	   
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
      tab_content_input='<textarea id="code'+code_editor_counter+'" cols="120" rows="30">body monitor control\n{\nforgetrate => "0.7";\n}</textarea>'+
    	                  '<input type="hidden" name="tabtype" value="codeEditor" />';              
    	 addTab('new');
    	 var editor = CodeMirror.fromTextArea('code'+code_editor_counter, {
    		    parserfile: ["cfsyntax.js", "parsecf.js"],
    		    stylesheet: "css/cfcolors.css",
    		    path: "scripts/",
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
	var path = "policy/get_list.php";
	$("#container_policies_id").load(path,{dir: 'policies/'}, function(data){	
	});
	
	$('ul#policies_list_new li a').live('click',function(){
        openfile($(this).attr('rel'),$(this).attr('id'));
        $('#'+$(this).attr('id')).hide('slow');
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
	        	   cur_file_path=data.information.server_path;
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
	           url: "policy/save_file_contents.php",
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
	 
	 
	 jQuery(document).ajaxStart(function(){
		 $('#spinner').css({top: '0' , left:$('body').width()/2 }).fadeIn();
		 });
	 jQuery(document).ajaxStop(function(){
		 $('#spinner').css({top: '0' , left:$('body').width()/2 }).fadeOut();
		 });*/// JavaScript Document