<div id="body">
    <div class="outerdiv">
        <div class="innerdiv">
            <div class="tables" style="overflow:auto; height: 350px;">
                <table id="repository-table">
                    <thead>
                        <tr>
                            <th>Repository Path</th>
                            <th>Delete</th>
                        </tr>
                    </thead>
                    <tbody>
                        <?php if (!empty($repoData)) {
                        ?>
                        <?php foreach ($repoData as $repository) {
                        ?><form method="post" action="/repository/edit">
                                <tr>
                                    <td><?php echo $repository['repoPath'] ?></td>
                                    <input type="hidden" name="repoPath" value="<?php echo $repository['repoPath'] ?>" />
                                    <td><input type="submit" name="submit" value="Delete" /></td>
                                </tr>
                            </form>
                    <?php } ?>
                    <?php } else {
                    ?>
                            <tr id="no-data-row">
                                <td colspan="3">No entries defined</td>
                            </tr>
                    <?php } ?>
                        </tbody>
                    </table>
                </div>
            </div>

            <div class="innerdiv">
            <?php require_once('add_new_repository.php'); ?>
            </div>
    </div>
</div>
<script type="text/javascript">
   
</script>
