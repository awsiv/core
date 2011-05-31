<div class="outerdiv grid_12">
    <div class="innerdiv">
        <p class="title">My Working of Notes</p>
     <?php
       $this->table->set_heading('Notes', 'Date');
        foreach($table as $rows)
        {
            $this->table->add_row($rows['working_on'], date('D F d h:i:s Y',$rows['date']));
        }
        echo "<div class=\"tables\">".$this->table->generate()."</div>";
      ?>
    </div>
</div>

