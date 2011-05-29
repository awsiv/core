<?php
   $this->table->set_heading('User', 'Repository', 'Revision','Date','Comments');
    foreach($table as $rows)
    {
        $this->table->add_row($rows['username'], $rows['repo'], $rows['version'], date('D F d h:i:s Y',$rows['date']),$rows['comment']);
    }
 echo "<div class=\"tables\">".$this->table->generate()."</div>";
?>
