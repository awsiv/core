<div id="body">
    <div class=" outerdiv">
        <div id="custom-tabs" style="margin:15px;">
            <ul>
                <li><a href="#tabs-1">Create</a></li>
                <li><?php echo anchor('virtualbundle/myBundles', 'List', array('id' => 'tabs-2')) ?></li>
            </ul>
            <div id="tabs-1" class="ui-corner-all">

                           <div id="infoMessage"><?php echo isset($message)?$message:"";?></div>
                           <div class="form">
                               <?php
                               $attributes = array('class' => '', 'id' => 'virtualbundleform');
                               echo form_open('virtualbundle/create', $attributes);
                               ?>
                               <p>
                                   <label for="vbname">Virtual Bundle Name:<span class="required">*</span></label>
                                   <input type="text" name="vbname" id="vbname"></input>
                               </p>
                               <p>
                                   <label for="hostclass">Host Class:<span class="required"></span></label>
                                   <input type="text" name="hostclass" id="hostclass"></input>
                               </p>
                               <p>
                                   <label for="description" class="aligntop">Description:<span class="required">*</span></label>
                                   <textarea id="description" name="description" rows="3" column="30" style="width:436px;height:155px"></textarea>
                               </p>
                               <p>
                                   <label for="promises">Promises:<span class="required">*</span></label>
                                   <?php echo anchor('widget/allpolicies', 'Add promises', array('id' => 'addpromises')) ?>
                               </p>
                               <ul id="promisesContainer">
                          
                               </ul>
                               <p  id="btnholder">
                               <?php echo form_submit(array('name' => 'submit', 'class' => 'btn', 'value' => 'Submit')); ?>
                               </p>
                           
                             <?php echo form_close();?>
                          </div>
            </div>
            <div id="tabs-2" class="ui-corner-all">
                
            </div>
        </div>
                
    </div>
</div>

<script type="text/javascript">
    $(document).ready(function(){
         var selectedpromisesContainer=$('#promisesContainer'),
             messageContainer=$('#infoMessage'),
             width=$(window).width(),
             height=$(window).height()-10,
             selPromises=new Array();
         var promise_url= '<?php echo site_url('promise/details/')?>';
         
        var $tabs=$( "#custom-tabs" ).tabs();
        
        $('#addpromises').policyfinder({
                        baseUrl: '<?php echo site_url() ?>',
                        defaultbehaviour:false,
                        showAddButton:true,
                        position:[width,0],
                        width:415,
                        height:height,
                        title:'Promises',
                        handleClicked:function(event,data){
                          if($.inArray(data.selectedHandleName,selPromises) == -1){
                           var li=$('<li>');
                           li.data('val',data.selectedHandleName);
                           var rmbtn=$('<span class="removePromise">');
                           var selectedPromise=$('<a class="promisetext" href="'+promise_url+'/'+data.selectedHandleName+'" target="_blank">'+data.selectedHandleName+'</a>');
                           li.append(selectedPromise).appendTo(selectedpromisesContainer);
                           //$(selectedPromise).ajaxyDialog({'title':'Promise Detail',dontOverrideTitle:true});
                           rmbtn.text('x').appendTo(li);
                           selPromises.push(data.selectedHandleName);
                          }
                        }
                    });
         
         
        $('span.removePromise').live('click',function(){
             var valremoved=$(this).parent().data('val');
             selPromises.splice( selPromises.indexOf( valremoved),1);
             $(this).parent().remove();
        });
        
    $('#virtualbundleform').submit(function(event){
         event.preventDefault();
         var promises=new Array(),
             form=this,
             inputs;
         
         selectedpromisesContainer.find("li").each(function() { 
            promises.push($(this).data('val'));
          });
          
         inputs={
             'promises':promises.join(','),
             'name':$('input:text[name=vbname]').val(),
             'description':$('textarea#description').val(),
             'hostclass':$('input:text[name=hostclass]').val()
                };
        $.ajax({
                url:$(form).attr('action'),
                data: inputs,
                type: 'POST',
                dataType:'html',
                success: function(resp, status, xhr) {
                    if (xhr.status != 0) {
                      messageContainer.html(resp);  
                    } else {
                        //stupid jquery calling this 'success', it's
                        //network unavailable.
                      messageContainer.html("<div class='ui-state-error' style='padding: 1em;'><p><span style='float: left; margin-right: 0.3em;' class='ui-icon ui-icon-alert'></span>Sorry, a software error has occured.</p><p>" + ": " + xhr.status + " "  + status+"</p><div>");
                    }
                },
                error: function(xhr, msg) {
                      messageContainer.html("<div class='ui-state-error' style='padding: 1em;'><p><span style='float: left; margin-right: 0.3em;' class='ui-icon ui-icon-alert'></span>Sorry, a software error has occured.</p><p>" + ": " + xhr.status + " "  + status+"</p><div>");
                }
            });
    });
    
    
    $('.inside').find('a').live('click',function(event){
        event.preventDefault();
        loadContent( this.href );
    });
    
    //when the no of rows displayed per page is change.
    $('.tables').find('form').live('submit',function(event){
        event.preventDefault();
        var url=$(this).attr('action')+'/rows/'+$('input:text[name=rows]').val();
        loadContent( url);
    });
    
    function loadContent( url ) {
        var selected = $tabs.tabs('option', 'selected');
        $tabs.tabs("url", selected , url ).tabs("load", selected);
        }   
    });
</script>