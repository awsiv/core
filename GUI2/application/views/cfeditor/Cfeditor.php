<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
#include(APPPATH .'views/includes/header'.EXT);
?>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title><?php echo $title ;?></title>
<link href="<?php echo get_cssdir();?>resets.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>960.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>Base.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_cssdir();?>jquery-ui-1.8.10.custom.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_scriptdir();?>/Cfeditor/lib/codemirror.css" rel="stylesheet" media="screen" />
<link href="<?php echo get_scriptdir();?>/Cfeditor/mode/cfengine/cfengine.css" rel="stylesheet" media="screen" />
<script src="<?php echo get_scriptdir();?>jquery-1.6.1.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir();?>jquery-ui-1.8.9.custom.min.js" type="text/javascript"></script>
<!--[if IE]><?php echo $this->carabiner->display('iefix'); ?><![endif]-->
<?php
$injected= isset($injected_item)?$injected_item:"" ;echo $injected;
$this->carabiner->display(); ?>
</head>
<body>
  <div id="wrapper" class="container_12">
     <div id="header">
              <div id="header_left"class="grid_3">
               <p class="sitetitle"><?php echo anchor("welcome/index",'cfengine mission portal') ?></p>
               </div>
         <div class="grid_6">
         <p id="repoinfo"> Working on :: <strong><?php echo $curreny_repo ?></strong></p>
         </div>
           
         <div id="header_right" class="grid_3">
                       <p id="userbox">
	                 revision:<strong><?php echo $revision ;?></strong>  &  Approvals:<strong><?php echo $total_approvals ?></strong>
                       </p>
                      <p class="clearright"></p>
             <div class="clear"></div>
          </div>
     </div><!--end of header-->

	<div id="leftPanel">
	 	<div class="ui-layout-content">
        <div id="Policies">
		  <div class="title">Nova Policies</div>
		    <div id="container_policies_id">
		    
		    </div>
		  </div>
		</div>
	</div>
	
	<div id="rightPanel">
	<div class="ui-layout-content">
	 <div id="tabs">
		  <ul>
			   <li>
			   <a href="#news">Welcome</a>
                           <!--<span class="ui-icon ui-icon-close">Remove Tab</span>-->
			   </li>
		  </ul>
		  <div id="news">
                  Welcome to the Cfengine Nova policy editor. Please click an existing policy file on the left, or "New" to start editing.
                  To know about the shortcuts available press <strong>ctrl+h (help)</strong>.
		  </div>
	 </div>

	</div>
</div>
          <div id="buttonpanel" class="dash">
                      <ul>
                          <li> <a href="#"  id="new">New</a></li>
                          <li> <a href="#"  id="save">Save</a></li>
                          <li> <a href="#"  id="checksyntax">Check syntax</a></li>
                          <li><a href="<?php echo site_url('repository/checkout')?>"  id="Checkout">Checkout</a></li>
                          <li><a href="#"  id="update">Update</a></li>
                          <li> <a href="#" id="Commit">Commit</a></li>
                          <li> <a href="<?php echo site_url('cfeditor/svnlogs')?>"  id="svnlogs">Log</a></li>
                      </ul>
                  </div>


    <div title="Tab data" id="dialog" style="display:none" class="dialog">
            <form>
                <fieldset class="ui-helper-reset">
                    <label for="tab_title">File Name</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="tab_title" name="tab_title" />
                    <textarea class="ui-widget-content ui-corner-all" id="tab_content" name="tab_content"></textarea>
                    <p class="dlgnotes"><strong>Notes:</strong> Relative path with filename should be given to create files inside sub directories</p>
                    <input type="hidden" id="event"></input>
                </fieldset>
            </form>
        </div>
        
    <div title="Save File" id="sfdialog" style="display:none" class="dialog">
        <span></span>
        <input type="hidden" id="tobesaved" ></input>
        <input type="hidden" id="tobesaved_name"></input>
    </div> 
<div id="commitdlg" style="display:none;" title="Commit file" class="dialog">
            <form>
                <fieldset class="ui-helper-reset">
                    <label for="tab_title" id="commentlbl">Comment</label>
                    <textarea class="ui-widget-content ui-corner-all" id="comments" name="comments"></textarea>
                    <input type="hidden" id="event"></input>
                    <input type="hidden" id="operation" ></input>
                    <input type="hidden" id="datatype" ></input>
                    <input type="hidden" id="cmtfile" ></input>
                </fieldset>
            </form>
   </div>
   
  <div id="svnlogtable" style="display:none;" class="dialog">
   </div>
  </div>

 <div id="keyboardHelp">
  <h2>Keyboard Shortcuts</h2>
  <table>
    <thead>
      <tr>
        <th>Shortcut</th>
        <th>Action</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>ctrl + s</td>
        <td>Save the current policy</td>
      </tr>
      <tr>
        <td>ctrl + n</td>
        <td>Open new policy tab</td>
      </tr>
      <tr>
        <td>ctrl + m</td>
        <td>
            compile the policy file
            <span>To compile a policy file , it need to be saved in the server</span>
        </td>
      </tr>
      <tr>
        <td>ctrl + q</td>
        <td>Close the current tab</td>
      </tr>
      <tr>
        <td>ctrl + space</td>
        <td>Auto-completion and suggest generation</td>
      </tr>
      <tr>
        <td>ctrl + z</td>
        <td>Undo</td>
      </tr>
      <tr>
        <td>esc</td>
        <td>get out of help</td>
      </tr>
    </tbody>
  </table>
</div>
<script src="<?php echo get_scriptdir()?>Cfeditor/jquery.layout.min-1.2.0.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir()?>jquery.jcryption-1.1.min.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir()?>Cfeditor/lib/codemirror.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir()?>Cfeditor/lib/shortcut.js" type="text/javascript"></script>
 <script src="<?php echo get_scriptdir()?>Cfeditor/mode/cfengine/cfsyntax.js" type="text/javascript"></script>
 <script src="<?php echo get_scriptdir()?>Cfeditor/mode/cfengine/cfengine.js" type="text/javascript"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>cfeditor.js"></script>
 <script src="<?php echo get_scriptdir()?>Cfeditor/mode/cfengine/cfautocomplete.js" type="text/javascript"></script>
</body>
</html>
