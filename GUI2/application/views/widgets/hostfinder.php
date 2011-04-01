<div>
    <form id="searchhost">
        <input type="text" name="search"/>
    </form>
</div>
<div>
    <ul id="searchby">
        <li>Host Name</li>
        <li>Ip Address</li>
        <li>Class</li>
    </ul>
    <div id="searchresult">
        <ul>
            <?php
            foreach($hostlist as $host)
            {
               echo "<li>.$host->id</li>";
            }

           ?>
        </ul>
    </div>
    <ul id="aplhaPaging">
      <?php
       foreach(range('a','z') as $letter)
        {
           echo "<li>.$letter.</li>";
        }
      ?>
    </ul>
</div>