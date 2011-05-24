<div>
    <div class="innerdiv">
        <div id=" stylized">
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
       <?php if (isset($errors) && is_array($errors) && !empty($errors)) { ?>
    <ul>
        <?php foreach ($errors as $error) { ?>
            <li> <?php echo $error ?> </li>
        <?php } ?>
    </ul>
<?php } ?>
<?php 
if ($this->session->flashdata('success')) { ?>
    <div class="success"><?php echo $this->session->flashdata('success'); ?></div> 
<?php } ?>
<div id="stylized">

    <form action="/repository/checkout/force" method="POST">
        <fieldset>

            <legend>Add new repository information and check out</legend>
            <label id="repoPath" for="repoPath">Repository Path :: </label>
            <input type="text" name="repoPath" value="<?php echo set_value('repoPath'); ?>" size="50" />

            <label for="username"> Username :: </label>
            <input autocomplete="off" id="username" type="text" name="username" value="<?php echo set_value('username'); ?>" size="50" />

            <label for="password">Password :: </label>
            <input autocomplete="off" id="password" type="password" name="password" value="<?php echo set_value('password'); ?>" size="50" />
            <input autocomplete="off" id="addnew" type="hidden" name="addnew" value="1"  />
            
            <label for="submit"></label>
            <input type="submit" value="Add" name="submit" />
        </fieldset>
        <br />
    </form>
</div>
    </div>
</div>

