<div id="body">
              <div class="left"></div>
              <div id="work" class="middle">
                  <div class="grid_8 push_2 header">
                      <h1>
                          Welcome to Cfengine.
                      </h1>
                      <p><?php
                      echo $this->lang->line('licese_to');
                      echo  anchor('welcome/license',cfpr_getlicense_owner(),array('class'=>'licenseinfo'));?></p>
                  </div>
                  <div class ="grid_8 push_2 dash">
                      <ul>
                          <li><?php echo anchor('welcome/status','status',array('class'=>'status'));?></li>
                          <li><?php echo anchor('welcome/knowledge','library',array('class'=>'library'));?></li>
                          <li><?php echo anchor('welcome/listhost','engineering',array('class'=>'engineering'));?></li>
                          <li><?php echo anchor('welcome/helm','planning',array('class'=>'planning'));?></li>
                          <p class="clearleft"></p>
                      </ul>
                  </div>
              </div>
              <div class="right"></div>
     <div class="clearboth"></div>
  </div>

