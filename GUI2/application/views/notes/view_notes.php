<div class="tables" style="overflow:auto; height: 250px;">
    <table class="table" id="notes-table">
        <thead class="nobgimage">
            <tr>
                <th>User</th>
                <th>Date</th>
                <th>Message</th>
            </tr>
        </thead>
        <tbody>
            <?php if (!empty($data)) {
            ?>
            <?php foreach ($data as $notes) {
            ?>
                    <tr>
                        <td><?php echo $notes->getuserId(); ?></td>
                        <td><?php echo $notes->getDate(); ?></td>
                        <td><?php echo $notes->getMessage(); ?></td>
                    </tr>
            <?php } ?>
            <?php } else {
 ?>
                <tr id="no-data-row">
                    <td colspan="3">No new notes</td>
                </tr>
<?php } ?>
        </tbody>
    </table>
</div>
<br />
<br />
<div   class="stylized" style="margin-left: 15px; margin-top: 5px;">
    <form id="notes-form" action="<?php echo $form_url; ?>" method="post">
        <fieldset>
            <?php if ( isset($updateMessage)) { ?>
           <div id="message-update" class="error"> <?php echo  $updateMessage; ?>  </div>
           <?php } ?>
  
            <legend> Add a note </legend>
            <textarea cols="70" rows="6" name="Message"></textarea>
            <input type="hidden" name="rid" value="<?php echo $rid; ?>"/>
            <input type="hidden" name="nid" value="<?php echo $nid; ?>"/>
            <input type="hidden" name="reporttype" value="<?php echo $reporttype; ?>"/>
            <input type="hidden" name="hash" value="<?php echo $hostkey; ?>"/>
            <br />
            <input type="submit" name="submitButton" value="Add note">
        </fieldset>    
    </form>
</div>