<div class="info">
    <p> You have already checked out the repository before. What do you want to do ? </p>
</div>
<?php if (isset($hasChanges) && is_array($hasChanges) && !empty($hasChanges)) { ?>
    <div class="warning">
        <p>Some files modified and not committed.</p>
        <ul>
            <?php foreach ($hasChanges as $file) { ?>
                <li> <?php echo $file; ?> </li>
            <?php } ?>
        </ul>    
    </div>

<?php } ?>
<div>
    <span class="btn"><a href="/repository/checkout/force" class="btn">Force checkout</a></span>
        <span class="btn"><a href="/cfeditor" target="_self" class="btn">Continue to policy editor</a></span>
    
</div>    