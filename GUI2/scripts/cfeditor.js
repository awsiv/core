// JavaScript Document
$(document).ready(function() {
	$("body").layout({
	defaults: {
	      fxName:               "slide"
	   ,  fxSpeed:               "slow"
	   ,  spacing_open:          14
	   },
           east:{
              fxName:                "slide"
	    ,  size:                 60
	    ,togglerLength_closed:    14
	    ,togglerAlign_closed:     "top"
            ,spacing_open:6
	   ,  spacing_closed:        6
	   ,  togglerLength_closed:  "100%"
	   ,  paneSelector:          "#buttonpanel"
	   ,  fxSettings:             {easing:""}
           }
	,  west: {
	      fxName:                "slide"
	   ,  size:                  240
	   ,togglerLength_closed:    21
	   ,togglerAlign_closed:     "top"
           ,spacing_open:6
	   ,  spacing_closed:      6
	   ,  togglerLength_closed:  "100%"
	   ,  paneSelector:          "#leftPanel"
	   ,  fxSettings:             {easing:""}
	   }
	,  center: {
		   paneSelector:         "#rightPanel"
	   }
	,  north: {
		   
		   paneSelector:         "#header"
	   ,  size:                  60
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

	//var default_dialog_width = 400;
	$('#buttonpanel-resizer').css('width', '5px');
	var tab_counter = 2;
		var code_editor_counter=0;//previously 1
		var tab_title_input="Untitled-";
		var tab_content_input="";
		
		var current_tab_title="";
		var current_tab_index="";
		var current_tab_id="tabs-1";
	
		$title_element="";
		var closetabs=false;
		var closetabindex="";
		var fileevent="";
		
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
	   
	   $('<img src="../images/ajax-loader.gif" id="spinner" />').css('position','absolute').hide().appendTo('body');
	   
	   
	   
	   //load policies from database into left hand side panel on list with id Policies_list
	/*var path = "cfeditor/get_list";
	$("#container_policies_id").load(path,{dir: 'policies'}, function(data){	
	});
	
	$('ul#policies_list_new li a').live('click',function(){
        openfile($(this).attr('rel'),$(this).attr('id'));
        $('#'+$(this).attr('id')).hide('slow');
	});*/
        loadfiletree();

        function loadfiletree()
        {
        $('#container_policies_id').fileTree({
        root: '',
        script: '/cfeditor/get_list',
        expandSpeed: 1000,
        collapseSpeed: 1000,
        multiFolder: false
        }, function(node) {

      var alreadyloaded=false;
           //check to see the file is already open in the tab
        $('#tabs').find('ul').find('li').each(function(){
            var tab=$(this).find('a').attr('href');
            //console.log($(tab).find('input[name="link"]').val())
              if($(tab).find('input[name="link"]').val()==node.id)
               {
                alreadyloaded=true;
                $(this).find('a').trigger('click')
                return false; //similar to break
               }
            });

        //if not open load the files into the tab
            if(!alreadyloaded)
                {
                     openfile(node.path,node.id);
                     $('#'+node.id).hide('slow');
                }
           // alert(node.path+" "+node.id);
        });
        }
	
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
    		    stylesheet: "../themes/default/css/cfcolors.css",
    		    path: "../scripts/Cfeditor/",
    		    lineNumbers: true,
    		    styleNumbers: styleLineNum,
    		    initCallback: function(e){initComplete(e);initResize(e);initChangeTracker(e);},
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
	           url: "cfeditor/get_contents",
	           data:({'file_path':file}),
	           dataType:'json',
	           success: function(data){
	        	  tab_title_input=data.filename;
	        	   //cur_file_path=data.information.server_path;
		           tab_content_input='<input type="hidden" name="tabtype" value="codeEditor" /><input type="hidden" name="link" value="'+link_id+'" />'+
		                             '<textarea id="code'+code_editor_counter+'" cols="120" rows="30">'+data.content+'</textarea>';
		                
		       	    //adding the new tab
                             addTab('existing');
			       	 var editor = CodeMirror.fromTextArea('code'+code_editor_counter, {
			       		    parserfile: ["cfsyntax.js", "parsecf.js"],
			       		    stylesheet: "../themes/default/css/cfcolors.css", // you may have to adjust the path
			       		    path: "../scripts/Cfeditor/",
			       		    lineNumbers: true,
			       		    styleNumbers: styleLineNum,
			       		    initCallback: function(e){initComplete(e);initResize(e);initChangeTracker(e);},
			       		    onChange: contentChanged,
			       		    continuousScanning: 500
			       		  });
                               
		   		code_editor_counter++;
                                $('.CodeMirror-wrapping',current_tab_id).get(0).style.height=$(current_tab_id).get(0).style.height;
		   		//addcontextmenu();//adding the context menu to the tab newly added
	             },
          error:function(data){
	            	
                        $.jnotify('An error occured while performing request','error');
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
                     var filepath=$('#'+link).attr('rel');
			 $.ajax({
			        type: "POST",
			        url: "cfeditor/compare_contents",
			        data:({'file':filepath,'newcontents':newcontents,'agent':agent}),
			        dataType:'json',
			        global: false,
			        success: function(data){
				        var status=data.status;
				        if(status=='changed')
				        { 
					      // $('#tobesaved_name',$sfd).val(tab_title);
                                               $('#tobesaved_name',$sfd).val(filepath);
					       $('#tobesaved',$sfd).val(newcontents);
					       $('span',$sfd).html('The contents of '+tab_title+' has been modified. Do you want to save the changes?');
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
			 fileevent = "closing";
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
		var agent=jQuery.uaMatch(navigator.userAgent).browser;
		 $.ajax({
                           type: "POST",
                           url: "cfeditor/save_contents",
                           data:({'file':$('#tab_title').val(), 'content':$('#tab_content').html(), 'filestats':'new', 'agent':agent}),
                           dataType:'json',
                           success: function(data){
			   if(data.status)
			   {
                               //reload the folder in the file tree
                               if(  $('#container_policies_id').find('a[rel="'+data.path+'/"]').length>0)
                                   {
                                       $('#container_policies_id').find('a[rel="'+data.path+'/"]').trigger('click');
                                   }
                                else
                                    {
                                         loadfiletree();
                                    }
                                
                                  if(fileevent!= "closing")
                                                         {
                                                             if( $('#container_policies_id').find('a[rel="'+data.path+'/'+data.title+'"]').length>0)
                                                                 {
                                                                      $('#container_policies_id').find('a[rel="'+data.path+'/'+data.title+'"]').trigger('click');
                                                                 }
                                                          $tabs.tabs('remove',current_tab_index);
                                                         }
                                                         
              
                              }
				else
			       {
				closetabs=false; 
				/*$confirmation.dialog({title: "Error", width:default_dialog_width});
	                        $confirmation.html('<span>An error occured: '+data.msg+'.</span>');
	                        $confirmation.dialog('open');*/
                                $.jnotify('An error occured while performing request'+data.msg,'error');
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
                         $.jnotify('Current tab cannot be saved',"error",5000);
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
                  var filepath=$('#'+file_type).attr('rel');
		 $.ajax({
	           type: "POST",
	           url: "cfeditor/save_contents",
	           data:({'file':filepath, 'content':newcontents,'filestats':'old', 'agent':agent}),
	           //data: "name="+current_tab_title+"&content="+html_stripped,
                    dataType:'json',
	           success: function(data){
                           $.jnotify(data.title+' was saved');
	             },
            error:function(data){
                          $.jnotify('An error occured'+data.msg,'error');
	             }
	       });
		 }
		 else
		 {
			 $("#tab_title").val(current_tab_title);
			 $("#tab_content").html(newcontents);
			 fileevent = "saving";
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
		 },
		 open: function() {
		 $(this).parent().find('.ui-dialog-buttonpane').find('button:first').focus()
		 }
	 });
	 	
	 
	 var $chkcnf=$('#checkoutconfirmation').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 resizable: false,
		 title:'Checkout',
		 buttons: {
		 'OK': function() {
			 $.ajax({
					   type: "POST",
					   url: "cfeditor/clear_dir",
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
                                                          $.jnotify('An error occured'+data.msg,'error');
						 }
				   });
			 $(this).dialog('close');
	      },
		'Cancel':function() {
	      $(this).dialog('close');
	      }
	  },
	  open: function() {
		 $(this).parent().find('.ui-dialog-buttonpane').find('button:first').focus();
		 }
	 });
	 
	  var $svnlogdlg = $('#svnlogtable').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 resizable: true,
		 height:500,
		 width:900,
		 title:'Change logs',
		 buttons: {
		 'Ok': function() {
	      $(this).dialog('close');
	      }
	 }
	 });
	 
	//save file dialogue to be displayed when user clicks the cross icon on the tab.
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
			  var agent=jQuery.uaMatch(navigator.userAgent).browser;
		    $.ajax({
	           type: "POST",
	           async:false,
	           url: "cfeditor/save_contents",
	           data:({'file':$('#tobesaved_name',this).val(),'content':$('#tobesaved',this).val(), 'filestats':'old', 'agent':agent}),
	           
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
                          $.jnotify('An error occured','error');
	             }
	       }); 
		    $(this).dialog('close');
	       },
       'Save': function() {
		 var agent=jQuery.uaMatch(navigator.userAgent).browser;
		 $.ajax({
	           type: "POST",
	           async:false,
	           url: "cfeditor/save_contents",
	           data:({'file':$('#tobesaved_name',this).val(),'content':$('#tobesaved',this).val(),'filestats':'old', 'agent':agent}),
	           //data: "name="+current_tab_title+"&content="+html_stripped,
                   dataType:'json',
	           success: function(data){
                       $.jnotify(data.title+" was saved");
	             },
              error:function(data){
                          $.jnotify("Error :: "+data.title+"was not saved","error");
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
	   .click(function(event) {
                 event.preventDefault();
			 if(code_editor_counter > 0)
		        {
                                 $.jnotify("Please close all open tabs first","error",5000);
                                 return;
			 }
			 else
			 {
                          $(this).ajaxyDialog({title:'SVN Checkout'}).ajaxyDialog('open');
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

          $('#update').click(function() {
		   if(code_editor_counter > 0)
		     {
				 $.jnotify("Please close all open tabs first","error",5000);
                                 return;
			 }
			 else
			 {
				 $.ajax({
						type: 'POST',
						url: "cfeditor/update",
                                                dataType:'json',
						success: function(data) {
                                                 if(data.status)
                                                                  {
                                                                     loadfiletree();
                                                                     $('#userbox').html('revision :: <strong> '+data.rev+'</strong> & Approvals :: <strong>'+data.total_approvals+'</strong>');
                                                                     $.jnotify("Sucessfully updated at revision "+ data.rev);
                                                                  }
                                                               else
                                                                   {
                                                                       $.jnotify("Update Failed : "+data.message,"error",5000);
                                                                   }
                                                },
					      error:function(data){
                                                 $.jnotify('Cannot Process the request '+data.status,'error');
						}
					});
                              
			 }
         });


 $('#svnlogs').ajaxyDialog({title:'SVN Logs', height:500});
       
	   $('#checksyntax')
	   .click(function() {
			$.ajax({
			type: "POST",
			async:false,
			url: "cfeditor/check_syntax",
			dataType:'json',
			success: function(data){
				if(data.result=="SUCCESS")
				{
                                 $.jnotify("The policy promises.cf has correct syntax");
				}
				else
				{
                                 $.jnotify(data.result.replace(/\n/g, "<br>"),"error");
				}
			},
		    error:function(data){
                              $.jnotify("An error occured.","error");
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
                        $.ajax({
						type: 'POST',
						url: "cfeditor/"+$("#operation").val(),
						data: {'op':$("#operation").val(),'file':$('#cmtfile').val(),'comments':$("#comments").val()},
                                                dataType:$("#datatype").val(),
						success: function(data) {
							if($("#operation").val()=='checkout')
							  {
								/*  if(data.status)
								  {
                                                                   loadfiletree();
                                                                   $('#repoinfo').html("working on :: "+$("#repo").val());
                                                                   $('#userbox').html('revision :: <strong> '+data.rev+'</strong> & Approvals :: <strong>'+data.total_approvals+'</strong>');
                                                                   $.jnotify("Checked out sucessfully.");
								  }
								  else
								  {
                                                                   $.jnotify("Checkout failed :: "+data.message,"error");
								  } */
							  }
						      else if($("#operation").val()=='commit')
							  {
                                                            //  $confirmation.dialog({title: $("#operation").val(), width:default_dialog_width});
								 if(data.status)
                                                                     {
                                                                         $.jnotify($('#cmtfile').val()+' Sucessfully committed');
                                                                     }
                                                                  else
                                                                      {
                                                                         $.jnotify(data.message,"error",5000);
                                                                      }              
							}
							$('#cmtfile').val('');
						},
					error:function(data){
                                                 $.jnotify('Cannot Process the request '+data.status,'error');
						   $('#cmtfile').val('')
						}
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
	 
$('.dialog').find('input').keypress(function(e) {
	if ((e.which && e.which == 13) || (e.keyCode && e.keyCode == 13)) {
		$(this).parent().parent().parent().parent().find('.ui-dialog-buttonpane').find('button:first').trigger('click'); 
		return false;
	}
});

	$('#repoText').live('click',function(){
		$('#usedRepo').css({top: '0' , left:'0', margin:'17px' , opacity:'0.9'});
		$('#usedRepo').slideToggle("slow");							 
	 });
	
	jQuery(document).ajaxStart(function(){
		 $('#spinner').css({top: '0' , left:$('body').width()/2}).fadeIn();
		 $('#repoText').css({top: '0' , left:'0'}).fadeOut();
		 });
	 jQuery(document).ajaxStop(function(){
		 $('#spinner').css({top: '0' , left:$('body').width()/2}).fadeOut();
		 if(closetabs)
		   {
			  $tabs.tabs('remove', closetabindex);
			  closetabindex="";
			  closetabs=false;
		   }
		 $('#repoText').css({top: '0' , left:'0'}).fadeIn();
		 });
	   
						   
});