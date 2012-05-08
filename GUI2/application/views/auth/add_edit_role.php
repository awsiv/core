<div id="users_roles_form"> 
<div id="formInfoMessage"><?php echo $message; ?></div>
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
        echo form_label('Description', $description['name'],array('class'=>'aligntop'));
        echo form_textarea($description);
       ?>
      </p>
      
<?php
      function get_array_from_string($value) {
          if(empty($value))
          {
              return null;
          }
          
            $value = str_replace(' ', '', $value);
            $arr = explode(',', $value);
         return $arr;
      }
       
      
      
      $partial_data = array();
      
      $partial_data['html_id']          = 'classes_inc_exc';
      $partial_data['embedded']         = true;  
      $partial_data['doNotShowButtons'] = true;     
      
      
      $partial_data['fields']['left']['name'] = 'crxi';
      
      if (!empty($crxi['value'])) {
          $partial_data['fields']['left']['values'] = get_array_from_string($crxi['value']);
      }
  
      
      $partial_data['fields']['right']['name'] = 'crxx';    
      
      if (!empty($crxx['value'])) {
          $partial_data['fields']['right']['values'] = get_array_from_string($crxx['value']);
      }
      ?>
<br />      
<h2>Classes</h2>      
<div id="classes_inc_exc">
    <?php $this->load->view('widgets/contextfinder', $partial_data); ?>
</div>
      
<?php       
      
      $partial_data = array();
      
      $partial_data['html_id']          = 'bundles_inc_exc';
      $partial_data['embedded']         = true;     
      $partial_data['doNotShowButtons'] = true;
      
      $partial_data['fields']['left']['name'] = 'brxi';
      
      $partial_data['fields']['left']['href'] = 'widget/bundlesNotAssignedToRole/' . $name['value'];
      
      if (!empty($brxi['value'])) {
          $partial_data['fields']['left']['values'] = get_array_from_string($brxi['value']);
      }
  
      
      $partial_data['fields']['right']['name'] = 'brxx';    
      
      $partial_data['fields']['right']['href'] = 'widget/bundlesNotAssignedToRole/' . $name['value'];
      
      if (!empty($brxx['value'])) {
          $partial_data['fields']['right']['values'] = get_array_from_string($brxx['value']);
      }
      ?>

        <h2>Bundles</h2>      
        <div id="bundles_inc_exc">
        <?php  $this->load->view('widgets/contextfinder', $partial_data); ?>
        </div>
        <span class="green_btn">
            <input type="submit" value="<?php echo $operation ?> role" class="green_btn" id="<?php echo strtolower($operation).'_role_btn'?>">
        </span>
    <?php echo form_close();?>
</div>  
</div>