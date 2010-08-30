<?php
/***************************/ 
/* Radek HULAN             */
/* http://hulan.info/blog/ */
/***************************/ 

require('makepdf.php');

if(isset($_POST['description']))
{
        $pdf =& new PDF();
        $pdf->run($_POST['title'], $_POST['description']);
//$pdf->Output("converted.pdf", "D");
}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html><head>
<meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
<title>Pdf Report</title>
<style type='text/css'>
  fieldset{padding:10px}
  body{font-size:small}
</style>
</head><body>
<h1>Generate pdf report</h1>
  <form name="pdfgen" method="post" action="<?php echo $_SERVER['PHP_SELF']; ?>">
  <div>
<!--    <fieldset><legend></legend> -->
    <p>Title: <input type="text" maxlength="50" name="title"></p>
    <p><label>Description:</label></p>
    <p><textarea name="description" cols="50" rows="15"></textarea></p>
    <p><input type="submit" value="Generate PDF"></p>
<!--    </fieldset> -->
  </div>
  </form>
</body></html>