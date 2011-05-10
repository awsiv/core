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
            <?php if (!empty($data)) { ?>
                <?php foreach ($data['data'] as $index => $d) { ?>
                    <tr>
                        <td><?php echo $d['user']; ?></td>
                        <td><?php echo date('D F d h:m:s Y', $d['date']); ?></td>
                        <td><?php echo $d['message']; ?></td>

                    </tr>
                <?php } ?>
            <?php } else { ?>
                <tr id="no-data-row">
                    <td colspan="3">No new notes</td>                
                </tr>
            <?php } ?>
        </tbody>
    </table>
</div>
<br />
<br />
<div   id="stylized" style="margin-left: 15px; margin-top: 5px;">
    <div id="message-update"> <?php echo isset($updateMessage) ? $updateMessage : '' ?>  </div>
    <form id="notes-form" action="<?php echo $form_url; ?>" method="post">
        <fieldset>
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