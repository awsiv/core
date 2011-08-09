 <div class="outerdiv grid_12">
        <div id="goals" class="innerdiv">
            <p class="title">Policy goals</p>
            <ul>
                <?php
                $goalsorted=array_msort($goals,array('name' => SORT_ASC),true);
                foreach ((array) $goalsorted as $goal) {
                    
                    $words = explode("_", $goal['name']);
                    echo "<li><span class=\"goal\">$words[0] $words[1]</span> - <span>".$goal['desc']."</span><span class=\"check\"></span></li>";
                }
                ?>
            </ul>
        </div>
 </div>
