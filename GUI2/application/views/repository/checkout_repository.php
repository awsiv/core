<div>

    <div class="stylized">
        <?php if (is_array($repoData) && !empty($repoData)) { ?>
            <form action="<?php echo site_url() ?>/repository/checkOutUrl" target="_self" method="POST">
                <fieldset>
                    <legend>Checkout</legend>
                    <label for="repourl">url: </label>
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
                No repositories are defined.
                Use <a target="_self" href ="<?php echo site_url() ?>/repository/manageRepository/">Manage Repository</a>
                to add a repository.
            </div>

        <?php } ?>
    </div>



    <?php require_once('add_new_repository.php'); ?>

    <div>
        <a href="<?php echo site_url() ?>/repository/managerepository" target="_self" class="slvbutton">Manage Repository</a>
    </div> 
</div>

