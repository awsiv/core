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

      <table cellpadding="0" cellspasing ="0" border="1"  id="maintable">
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
                              <?php echo roles_checkbox($crxi['value'], 'crxi', 'classlist itemlist assignedList'); ?>                
                          <?php } else { ?>
                              <ul id="crxi" class="classlist itemlist assignedList"><li class="empty">No classes assigned</li></ul>
                          <?php } ?>
                      </div>
                  </div>            
              </td>
              <td class="arrows">
                  <a href="javascript:void(0);" class="move_btn" dest="crxi" sourse="classList">&laquo; Left</a><br /><br />
                  <a href="javascript:void(0);" class="move_btn" dest="classList" sourse="crxi">Right &raquo;</a><br /><br />
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
                              <?php echo roles_checkbox($crxx['value'], 'crxx', 'classlist itemlist excludedList'); ?>                          
                          <?php } else { ?>
                              <ul id="crxx" class="classlist itemlist excludedList"><li class="empty">No classes assigned</li></ul>
                          <?php } ?>
                      </div>
                  </div>   

              </td>
              <td></td>
          </tr>
          <tr><td colspan="3"><hr /></td></tr>
          <tr>
              <td class="edit_cell">
                  <div class="DragNDrop assigned dialog_box_style ui-dialog">
                      <div class="ui-dialog-titlebar"><h6>Assigned bundlerx's</h6></div>
                      <div class="itemwrapper assigneditems" id="">
                          <?php if (!empty($brxi['value'])) { ?>
                              <?php echo roles_checkbox($brxi['value'], 'brxi', 'bundlelist itemlist assignedList'); ?>                       
                          <?php } else { ?>
                              <ul id="brxi" class="bundlelist itemlist assignedList"><li class="empty">No bundles assigned</li></ul>
                          <?php } ?>
                      </div>
                  </div>            
              </td>
              <td class="arrows">
                  <a href="javascript:void(0);" class="move_btn" dest="brxi" sourse="bundlesList">&laquo; Left</a><br /><br />
                  <a href="javascript:void(0);" class="move_btn" dest="bundlesList" sourse="brxi">Right &raquo;</a><br /><br />
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
                              <?php echo roles_checkbox($brxx['value'], 'brxx', 'bundlelist itemlist excludedList'); ?>                       
                          <?php } else { ?>
                              <ul id="brxi" class="bundlelist itemlist assignedList"><li class="empty">No bundles assigned</li></ul>
                          <?php } ?>
                      </div>
                  </div>            
              </td>
              
              <td></td>
          </tr>
          <tr>
              <td colspan="3">
                  <p id="btnholder">
                      <?php echo form_submit(array('name' => 'submit', 'value' => $operation, 'class' => 'btn')); ?>
                  </p>
              </td>
          </tr> 
      </table>


    <?php echo form_close();?>
</div>
