<?php if (!$goalsError)
{
    if (is_array($goals) && !empty($goals))
    { ?>
        <ul>
            <?php
            $val = 0;
            $goalsorted = array_msort($goals, array('name' => SORT_ASC), true);
            foreach ((array) $goalsorted as $goal)
            {
                if ($val == 5)
                {
                    break;
                }
                $words = explode("::", $goal['name']);

                if (!array_key_exists(1, $words))
                {
                    $text = $words[0];
                }
                else
                {
                    $text = $words[1];
                }
                echo "<li><span class=\"goal\">$text</span> - <span>" . $goal['desc'] . "</span><span class=\"check\"></span></li>";
                $val++;
            }
            ?>
        </ul>
        <p class="morebtnpane"><span class="morebtn"><?php echo anchor('welcome/goals', 'More...') ?></span></p>
    <?php }
    else
    { ?>
        <p  style="padding:10px;">No goals found.</p>
    <?php }
}
else
{ ?>
    <p style="padding:10px;font-weight: bold;color:#D95252;">Cannot retrieving goals due to some internal error. Please check logs for more details.</p>
<?php } ?>