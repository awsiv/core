<div id="edit_form">
<div id="infoMessage"><?php echo $message;?></div>
<div class="form">
    <?php echo form_open("auth/manage_role/".$this->uri->segment(3).'/'.$this->uri->segment(4),array('id'=>  strtolower($operation).'_role'));?>
      <p>
      <?php
       echo form_label('Role Name',$name['name']);
       
       // edit role - do not allow user to change the name
       if ($op == 'edit') {  
           $name['readonly'] = 'readonly';
       }
       echo form_input($name);
       ?>
      </p>
      
      <p>
      <?php  
        echo form_label('Description',$description['name'],array('class'=>'aligntop'));
        echo form_textarea($description);
       ?>
      </p>

      <table cellpadding="0" cellspasing ="0" border="0"  id="maintable">
          <tr>
              <td colspan="3">  
                  <h2>Classes</h2>
                  <?php
                  ?></td>
          </tr>     

          <tr>
              <td class="edit_cell">
                  <div class="DragNDrop assigned dialog_box_style ui-dialog">
                      <div class="ui-dialog-titlebar"><h6>Assigned classes</h6></div>
                      <div class="itemwrapper assigneditems" id="">
                          <?php if (!empty($crxi['value'])) { ?>
                              <?php echo create_html_list_from_string($crxi['value'], 'crxi[]','crxi', 'classlist itemlist assignedList'); ?>                
                          <?php } else { ?>
                              <ul id="crxi" class="classlist itemlist assignedList empty_list"></ul>
                              <div class="empty_list_warning">No classes assigned</div>
                          <?php } ?>
                      </div>
                  </div>            
              </td>
              <td class="arrows">
                  <a href="javascript:void(0);" class="left" dest="crxi" source="classList" title="&laquo; Left">&nbsp;</a>
                  <a href="javascript:void(0);" class="right" dest="classList" source="crxi" title="Right &raquo;">&nbsp;</a>
              </td>
              <td rowspan="2">
                  <div id="finderWrapper" class="all_classes dialog_box_style DragNDrop">
                      <div id="clfres" class="ui-dialog">
                          <div class="ui-dialog-titlebar ui-widget-header ui-corner-all ui-helper-clearfix"><h6>Available classes</h6></div>
                          <div class="formItem addRegex"><input id="classRegexpText" type="text"> <a href="javascript:void(0);" id="addClassRegexp" style="font-size: 26px">+</a></div>
                          <div id="classList_wrapper" style="display:block" class="result ui-dialog-content" title="Classes">
                              <div id=""><?php echo anchor('widget/allclasses', $this->lang->line('class'), array('class' => 'classes ', 'id' => 'classList')); ?></div>
                              <ul id="classList"></ul>
                          </div>
                      </div>
                  </div>
              </td>   
          </tr>            
          <tr>
              <td class="edit_cell">

                  <div class="DragNDrop excluded customdlg dialog_box_style ui-dialog">
                      <div class="ui-dialog-titlebar"><h6>Excluded classes</h6></div>

                      <div class="itemwrapper excludeditems " id="">
                          <?php if (!empty($crxx['value'])) { ?>
                              <?php echo create_html_list_from_string($crxx['value'], 'crxx[]','crxx', 'classlist itemlist excludedList'); ?>                          
                          <?php } else { ?>
                              <ul id="crxx" class="classlist itemlist excludedList empty_list"></ul>
                              <div class="empty_list_warning">No classes assigned</div>
                          <?php } ?>
                      </div>
                  </div>   

              </td>
               <td class="arrows">
                  <a href="javascript:void(0);" class="left" dest="crxx" source="classList" title="&laquo; Left">&nbsp;</a>
                  <a href="javascript:void(0);" class="right" dest="classList" source="crxx" title="Right &raquo;">&nbsp;</a>
              </td>
          </tr>
          <tr><td colspan="3"><hr /></td></tr>
          <tr>
              <td class="edit_cell">
                  <div class="DragNDrop assigned dialog_box_style ui-dialog">
                      <div class="ui-dialog-titlebar"><h6>Assigned bundlerx's</h6></div>
                      <div class="itemwrapper assigneditems" id="">
                          <?php if (!empty($brxi['value'])) { ?>
                              <?php echo create_html_list_from_string($brxi['value'], 'brxi[]', 'brxi', 'bundlelist itemlist assignedList'); ?>                       
                          <?php } else { ?>
                              <ul id="brxi" class="bundlelist itemlist assignedList empty_list"></ul>
                              <div class="empty_list_warning">No bundles assigned</div>
                          <?php } ?>
                      </div>
                  </div>            
              </td>
              <td class="arrows">
                  <a href="javascript:void(0);" class="left" dest="brxi" source="bundlessList" title="&laquo; Left">&nbsp;</a>
                  <a href="javascript:void(0);" class="right" dest="bundlessList" source="brxi" title="Right &raquo;">&nbsp;</a>
              </td>
              <td rowspan="2">
                  <div id="finderWrapper" class="all_classes dialog_box_style DragNDrop">
                      <div id="" class="ui-dialog">
                          <div class="ui-dialog-titlebar ui-widget-header ui-corner-all ui-helper-clearfix"><h6>Available Bundles</h6></div>
                          <div class="formItem addRegex"><input id="classRegexpText" type="text"> <a href="javascript:void(0);" id="addClassRegexp" style="font-size: 26px">+</a></div>
                          <div id="bundlessList_wrapper" style="display:block" class="result ui-dialog-content" title="Classes">
                              <div id=""><?php echo anchor('widget/bundlesNotAssignedToRole/' . $name['value'], $this->lang->line('class'), array('class' => 'classes ', 'id' => 'bundlessList')); ?></div>
                          </div>
                      </div>
                  </div>
              </td>  
          </tr>

          <tr>


              <td class="edit_cell">
                  <div class="DragNDrop assigned dialog_box_style ui-dialog">
                      <div class="ui-dialog-titlebar"><h6>Excluded bundlerx's</h6></div>
                      <div class="itemwrapper assigneditems" id="">
                          <?php if (!empty($brxx['value'])) { ?>
                              <?php echo create_html_list_from_string($brxx['value'], 'brxx[]' ,'brxx', 'bundlelist itemlist excludedList'); ?>                       
                          <?php } else { ?>
                              <ul id="brxx" class="bundlelist itemlist assignedList empty_list"></ul>
                              <div class="empty_list_warning">No bundles assigned</div>
                          <?php } ?>
                      </div>
                  </div>            
              </td>
              <td class="arrows">
                  <a href="javascript:void(0);" class="left" dest="brxx" source="bundlessList" title="&laquo; Left"></a>
                  <a href="javascript:void(0);" class="right" dest="bundlessList" source="brxx" title="Right &raquo;"></a>
              </td>
          </tr>
          <tr>
              <td colspan="3">
                  <p id="btnholder">
           
                      <span class="green_btn">
                          <input type="submit" value="<?php echo $operation ?>" class="green_btn" id="<?php echo strtolower($operation).'_role'?>">
                      </span>
                  </p>
              </td>
          </tr> 
      </table>


    <?php echo form_close();?>
</div>
</div>