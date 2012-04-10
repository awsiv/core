<div class="outerdiv grid_12">
    <div id="goals" class="innerdiv">
        <p class="title">Policy goals</p>
        <ul>
            <?php
            $goalsorted = array_msort($goals, array('name' => SORT_ASC), true);
            foreach ((array) $goalsorted as $goal) {
                $words = explode("::", $goal['name']);
                if (!array_key_exists(1, $words)) {
                    $text = $words[0];
                } else {
                    $text = $words[1];
                }
                echo "<li><span class=\"goal\">$text</span> - <span>" . $goal['desc'] . "</span><span class=\"check\"></span></li>";
            }
            ?>
        </ul>
    </div>
</div>
