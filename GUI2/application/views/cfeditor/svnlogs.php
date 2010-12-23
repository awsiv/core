<div class="tables">
   <table><thead><th>Revision</th><th>Author</th><th>Message</th><th>Date</th><th>File</th></thead><tbody>
	<?php
		if(count($logs) >0)
		 {
			foreach ($logs as $rows)
			{
				echo "<tr>";
				foreach ($rows as $column=>$value)
				{
							 if($column=="paths")
							   {
								  echo "<td><table>";
								   foreach ($value as $innerrows)
									{
									   echo "<tr>";
									   foreach ($innerrows as $innercol=>$colval)
									   {
										 echo "<td>$colval</td>";  
									   }
									   echo "</tr>";
									}
									echo "</table></td>";
							   }
							   else
							   {
								   echo "<td>$value</td>";
							   }
				 
				}
				echo "</tr>";
			}
		 }
		 else
		 {
			 echo "<tr><td colspan=\"5\">NO records found</td></tr>";
		 }
	?>
   </tbody>
 </table>
<div>