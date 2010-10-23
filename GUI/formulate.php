<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("map intentions to promises","normal");

?>

<div id="info">
<table>
<tr>
<td>

<form>
<p>Enter, topic by topic, your verbal description of policy.</p>


<table>
<tr><th>Verbal intention</a></th><th>Technical promise</th></tr>
<tr>
<td>
<form>
<ol>
<li>Choose a <b>short unique name</b> (topic ID) for this item: (<a href="">why?</a>)<br>
<textarea rows="1" cols="80">Memory requirement</textarea>

<li>Enter the requirement as clearly and concisely as possible.
(Refer to other item names if you like)<p>
<textarea rows="10" cols="80">A web server machine must have 2GB RAM.</textarea>
<button>Add new intention</button>
</form>
</ol>
</td>
<td>
<h2>Cfengine Promises that implement this:</h2>
<ul>
<li><img src="/green.png">&nbsp;<a href="">Check_RAM</a> in bundle <a href="">Hardware</a> 
<li><img src="/yellow.png">&nbsp;<a href="">Apache_memory_watch</a> in bundle <a href="">web_server_monitoring</a> 
</ul>
<button>Add new promise</button>
</td>
</tr>
</table>
<p>
<ul>
<li>Topics that might be related:

<ul>
<li><a href="">Apache_memory_watch</a>
</ul>

</ul>
</td>
</tr>
</table>

</div>
     
<?php
cfpr_footer();
?>