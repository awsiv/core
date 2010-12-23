<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
include(APPPATH .'views/includes/header'.EXT);
?>
    <div id="editor">
	<div id="leftPanel">
	 	<div class="ui-layout-content">
        <div id="Policies">
		  <div class="title">Nova Policies</div>
		    <div id="container_policies_id">
		    
		    </div>
		  </div>
		  <div id="menu">
			<a href="#" class="menuitem" id="new"><img class="icontext" src="<?php echo get_imagedir() ?>new.png"/>New</a>
			  <a href="#" class="menuitem" id="save"><img class="icontext" src="<?php echo get_imagedir() ?>save.png">Save</a>
			  <a href="#" class="menuitem" id="checksyntax"><img class="icontext" src="<?php echo get_imagedir() ?>check_syntax.png"/>Check syntax</a>
              <a href="#" class="menuitem" id="Checkout"><img class="icontext" src="<?php echo get_imagedir() ?>checkout.png"/>Checkout</span></a>
			  <a href="#" class="menuitem" id="update"><img class="icontext" src="<?php echo get_imagedir() ?>update.png"/>Update</span></a>
			  <a href="#" class="menuitem" id="Commit"><img class="icontext" src="<?php echo get_imagedir() ?>commit.png"/>Commit</span></a>
              <a href="#" class="menuitem" id="svnlogs"><img class="icontext" src="<?php echo get_imagedir() ?>log.png"/>Log</span></a>
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
		  </div>
	 </div>

	</div>
</div>
</div> <!--hello-->

    <div title="Tab data" id="dialog" style="display:none" class="dialog">
            <form>
                <fieldset class="ui-helper-reset">
                    <label for="tab_title">File Name</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="tab_title" name="tab_title" />
         
                    <textarea class="ui-widget-content ui-corner-all" id="tab_content" name="tab_content"></textarea>
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
                    <label for="tab_title">User name:</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="username" />
                    <label for="tab_title">Password:</label>
                    <input type="password" class="ui-widget-content ui-corner-all" value="" id="password" />
                    <label for="tab_title" id="repolbl">Repository</label>
                    <input type="text" class="ui-widget-content ui-corner-all" value="" id="repo" />
                    <label for="tab_title" id="commentlbl">Comment</label>
                    <textarea class="ui-widget-content ui-corner-all" id="comments" name="comments"></textarea>
                    <input type="hidden" id="event"></input>
                    <input type="hidden" id="operation" ></input>
                    <input type="hidden" id="datatype" ></input>
                    <input type="hidden" id="cmtfile" ></input>
                </fieldset>
            </form>
   </div>
   
   <div id="confirmation" style="display:none">
   </div>
   <div id="svnlogtable" style="display:none;" class="dialog">
   </div>
   <div id="checkoutconfirmation" style="display:none" class="dialog">
   </div>
   <div id="usedRepo" style="display:none">  
   </div>
<script src="<?php echo get_scriptdir()?>Cfeditor/jquery.layout.min-1.2.0.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir()?>Cfeditor/codemirror.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir()?>Cfeditor/cf.js" type="text/javascript"></script>
<script src="<?php echo get_scriptdir()?>jquery.jcryption-1.1.min.js" type="text/javascript"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>jquery.fancybox-1.3.1.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>cfeditor.js"></script>
</body>
</html>
