<div class="stylized">
    <form action="/repository/approveRepoAction" method="POST">
        <fieldset>
            <legend>Approve Policy</legend>
            <p>
                <label  for="repolist">Repository :: </label>
                <?php
                echo form_dropdown('repository', array_combine($reposoptions, $reposoptions), $reposoptions[0], 'id="repolist"');
                ?>
                <br />
            </p>

            <p>
                <label for="revisions"> Revisions :: </label>
                <span id="revisions">
                    <?php
                    if (is_array($revs) && count($revs) > 0) {
                        echo form_dropdown('revision', array_combine($revs, $revs), $revs[0]);
                    } else {
                        echo "";
                    }
                    ?>
                </span>
                <br />
            </p>

            <p>
                <label for="comments"> Comments :: </label>
                <textarea  id="comments" name="comments" placeholder="comments" cols="40" rows="5"> </textarea>
                <br />
            </p>
            <label for="submit"></label>
            <input type="submit" id="button" value=" Approve " class="slvbutton"/>
<?php echo anchor('repository/approvedpolicies', 'View approved policies', array('target' => "_self", "class" => "slvbutton")) ?>
        </fieldset>
        <br />
    </form>
</div>
<script type="text/javascript">
    $(document).ready(function(){
        $('#repolist').change(function(){
            var selected_repo=$(this).find("option:selected").text();
            $('#revisions').load('/repository/getListofRev',{repo:selected_repo});
        });
        $("#comments").live('focusin',function(){$(this).autoGrow();});
    });
</script>
