<?php
#
# This file is (C) Cfengine AS. All rights reserved
#

cfpr_header("add to knowledge bank","normal");

?>
<div id="info">

<form>

<table>
<tr>
<th><t2>Propose new topic</t2></th>
</tr>
<tr>
<td>
<form>
<ol>
<li>Choose a <b>short unique name</b> (topic ID) for this item: (<a href="">why?</a>)<br>
<textarea rows="1" cols="80">Memory requirement</textarea>

<li>Choose a category for this promise (a suitable box to keep it in)<br>

<select>
<option>System</option>
</select>
  other: <input name="">

<li>A longer comment or description<p>
<textarea rows="10" cols="80">A web server machine must have 2GB RAM.</textarea>
<button>Add new topic</button>

<li>Add a URL: <input name="url">
</ol>
</tr>
<tr>
<th><h2>Relate (associate) to other matters</h2></th>
</tr>
<tr>
<td>
<ol>
<li>How is this related (associated) to other matters in the knowledge bank?
<p>
MY TOPIC  <select>
<option>relies on</option>
<option>is relied on by</option>
</select>
<select>
<option>topic 1</option>
<option>topic 2</option>
</select>

</form>
</ol>
</td>
</tr>
</table>

</div>
     
<?php
cfpr_footer();
?>