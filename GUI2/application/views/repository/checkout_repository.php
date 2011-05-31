<div>
    <div class="innerdiv">
        <div class=" stylized">
            <form action="/repository/checkOutUrl" target="_self" method="POST">
                <fieldset>
                    <legend>Checkout</legend>
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
        </div>
    </div>
    <div class="innerdiv">
         <?php require_once('add_new_repository.php'); ?>
    </div>  
</div>

