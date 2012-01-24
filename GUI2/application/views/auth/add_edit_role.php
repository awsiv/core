<div id="infoMessage"><?php echo $message;?></div>
<div class="form">
    <?php echo form_open("auth/manage_role/".$this->uri->segment(3).'/'.$this->uri->segment(4),array('id'=>$operation.'_role'));?>
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

      <p>
      <?php
        echo form_label("Include classrx's (hosts)", $crxi['name'], array('class'=> 'regex'));
        echo form_input($crxi);
       ?>
      </p>    
      <p>
      <?php
        echo form_label("Exclude classrx's (hosts)", $crxx['name'], array('class'=> 'regex'));
        echo form_input($crxx);
       ?>
      </p>  
      <p>
      <?php
        echo form_label("Include bundlerx's", $brxi['name'],  array('class'=> 'regex'));
        echo form_input($brxi);
       ?>          
      </p>  
      <p>
      <?php
        echo form_label("Exclude bundlerx's", $brxx['name'],  array('class'=> 'regex'));
        echo form_input($brxx);
       ?>          
      </p>  
          
      <p id="btnholder">
      <?php echo form_submit(array('name'=>'submit','value'=>$operation,'class'=>'btn'));?>
      </p>


    <?php echo form_close();?>
</div>