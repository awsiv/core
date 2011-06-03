<div>

    <div class="stylized">
        <?php if (is_array($repoData) && !empty($repoData)) { ?>
            <form action="/repository/checkOutUrl" target="_self" method="POST">
                <fieldset>
                    <legend>Checkout</legend>
                    <label for="repourl">url :: </label>
                    <select name="repourl"> <?php if (!empty($repoData)) {
                ?>
                            <?php foreach ($repoData as $repository) {
                                ?><option><?php echo $repository['repoPath'] ?></option>
                            <?php } ?>
                        <?php } ?> </select>
                    <br />
                    <label for="submit"></label>
                    <input type="submit" value="Checkout" name="submit" />
                </fieldset>
                <br />
            </form>
        <?php } else { ?>
        
        <div class="innerdiv">
            You don't have any repository defined.  Manage you repositories. <a target="_self" href ="/repository/manageRepository/">Go</a>
        </div>

        <?php } ?>
    </div>



    <?php require_once('add_new_repository.php'); ?>

</div>

