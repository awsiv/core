<?php
$comment_id = $_GET['comment_id'];
$hostkey = $_GET['hostkey'];
$report_type = $_GET['report_type'];
$report_data = $_GET['report_data'];
$time =$_GET['time'];
$op=$_POST['op'];


if($op == "addcomment")
{
/* call add comment */
   $report_type = $_POST['report_type'];
   $report_data = $_POST['report_data'];
   $hostkey = $_POST['hostkey'];
   $comment_id = $_POST['comment_id'];
   $time = $_POST['time'];
   $username = $_POST['username'];
   $comment_text = $_POST['comment_text'];
   $report_id = $_POST['report_id'];
   
 cfpr_comment_add($hostkey,$report_id,$comment_id,$report_type,$report_data,$username,$time,$comment_text);
}

if($comment_id == "NO_COMMENT")
{
/* show form */
 echo "<div id=\"addComment\">
  <form name =\"addCommentForm1\" action=\"comments.php\" method=\"post\">
  <p><label>Username: </label><input type=\"text\" name=\"username\"></p>
  <p><label>Report Data: </label><input type=\"text\" name=\"report_data\"></p>
  <p> <textarea name=\"comment_text\" rows=\"10\" cols=\"50\"> </textarea> </p>
  <input type=\"hidden\" name=\"hostkey\" id=\"hostkey\" value=\"$hostkey\"/>
  <input type=\"hidden\" name=\"op\" id=\"op\" value=\"addcomment\"/>
  <input type=\"hidden\" name=\"report_type\" id=\"report_type\" value=\"$report_type\"/>
  <input type=\"hidden\" name=\"time\" id=\"time\" value=\"$time\"/>
  <input type=\"hidden\" name=\"time\" id=\"time\" value=\"$time\"/>
  <input type=\"hidden\" name=\"report_id\" id=\"report_id\" value=\"$report_data\"/>
  <input type=\"submit\" value=\"Comment!\" name=\"submitComment1\">
  </form>
 </div>
"; /* echo ends here */
}
else 
{
/* show comment*/
  if($comment_id != "")
  {
   $comment = cfpr_comment_query('',$comment_id,'',-1,-1);
   echo $comment;	
  }

/* show form */
$time=time();

 echo "<div id=\"addComment\">
  <form name =\"addCommentForm1\" action=\"comments.php\" method=\"post\">
  <p><label>Username: </label><input type=\"text\" name=\"username\"></p>
  <p><label>Report Data: </label><input type=\"text\" name=\"report_data\"></p>
  <p> <textarea name=\"comment_text\" rows=\"10\" cols=\"50\"> </textarea> </p>
  <input type=\"hidden\" name=\"hostkey\" id=\"hostkey\" value=\"$hostkey\"/>
  <input type=\"hidden\" name=\"op\" id=\"op\" value=\"addcomment\"/>
  <input type=\"hidden\" name=\"report_type\" id=\"report_type\" value=\"2\"/>
  <input type=\"hidden\" name=\"time\" id=\"time\" value=\"$time\"/>
  <input type=\"hidden\" name=\"comment_id\" id=\"comment_id\" value=\"$comment_id\"/>
  <input type=\"submit\" value=\"Comment!\" name=\"submitComment1\">
  </form>
 </div>
";

}

?>