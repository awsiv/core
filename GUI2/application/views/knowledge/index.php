<div id="body">
    <div class="left"></div>
    <div class="middle minwidth98">

        <div class="innerdiv" style="margin:15px 8px 5px 5px;">
            <p class="title">Search for any topic below : </p>
            <div style="padding:10px;text-align: center;">
                <form action="<?php echo site_url('knowledge/knowledgeSearch') ?>" method="post">
                    <label id="searchbox">
                        <input type="text" name="search" value="Search in knowledge map" onfocus="if(this.value==this.defaultValue)this.value='';" onblur="if(this.value=='')this.value=this.defaultValue;"/>
                    </label>
                </form>
            </div>

        </div>

        <div class="innerdiv" style="margin:15px 8px 5px 5px;">
            <p class="title">Useful Links </p>
            <div style="padding:10px;">
                <ul>
                    <li>Docs links</li>
                    <li>Online links</li>
                    <li>more links..</li>
                </ul>
            </div>

        </div>
    </div>
    <div class="right"></div>
    <div class="clearboth"></div>
</div>
<div class="clear"></div>


