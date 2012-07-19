<div class="outerdiv">
<div class=" innerdiv ">
    <div class="panelhead">MY preferences</div>
    <div id="infoMessage"><?php echo $message;?></div>
    <div class="form">
    <?php
            $attributes = array('class' => '', 'id' => '');
            echo form_open('settings/preferences/'.$op, $attributes); ?>
     <p>
      <label for="tooltips">Show tooltips <span class="required">*</span></label>
        <?php // echo form_error('mode'); ?>

                <?php // Change or Add the radio values/labels/css classes to suit your needs ?>
                     <input id="opt1" name="tooltips" type="radio" class="" value="tips_on" <?php echo (isset($tips_on))?$tips_on:$this->form_validation->set_radio('tooltips', 'tips_on') ; ?> checked="checked"/>
        		<label for="tooltips" class="">On</label>

        		<input id="opt2" name="tooltips" type="radio" class="" value="tips_off" <?php echo (isset($tips_off))?$tips_off:$this->form_validation->set_radio('tooltips', 'tips_off'); ?> />
        		<label for="tooltips" class="">Off</label>
     </p>
      <p>
        <label for="num_rows">Default number of rows shown in reports table <span class="required">*</span></label>
        <?php
         $arrayListRows = array("25"=>"25","50"=>"50","100"=>"100","250"=>"250","500"=>"500","1000"=>"1000");
         echo form_dropdown('num_rows', $arrayListRows,array(0=>$num_rows));
        ?>
     </p>

     <p  id="btnholder">
        <?php echo form_submit( array('name'=>'submit','class'=>'btn','value'=>'Submit')); ?>
    </p>

    <?php echo form_close(); ?>
    </div>
  </div>
</div>