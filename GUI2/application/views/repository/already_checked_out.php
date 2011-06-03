<div class="info">
    <p> You have already checked out the repository before. What do you want to do ? </p>
</div>
<?php if (is_array($hasChanges) && !empty($hasChanges)) { ?>
    <div class="warning">
        <p>Some files modified and not committed.</p>
        <ul>
            <?php foreach ($hasChanges as $file) { ?>
                <li> <?php echo $file; ?> </li>
            <?php } ?>
        </ul>    
    </div>

<?php } ?>
<div class="">
    <ul>
        <li><a href="/repository/checkout/force" class="btn">Force checkout</a></li>
        <li><a href="/cfeditor" target="_self" class="btn">Continue to policy editor</a></li>
    </ul> 
</div>    