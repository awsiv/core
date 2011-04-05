<html>
    <head>
     <title>Host Finder</title>
     <style rel="stylesheet">
         
         #container{
             width:700px;
         }
     </style>
    </head>
    <body>
        <div id="container" class="container_12">
        <div class="grid_12">
        <form id="searchhost" action="<?php echo site_url('widget/search_by_hostname');?>">
            <input type="text" name="search" value="Search by hostname"/>
        </form>
       </div>
        <div class="clear"></div>
    <div class="grid_12">
        <ul id="searchby" class="grid_4">
            <li id="search_hostname">Host Name</li>
            <li id="search_ip">Ip Address</li>
            <li id="search_class">Class</li>
        </ul>
        <div id="searchresult" class="grid_7">
            <ul>
                <?php
                foreach($hostlist as $host)
                {
                   echo "<li>.$host->id</li>";
                }
               ?>
            </ul>
        </div>
        <ul id="aplhaPaging" class="grid_1">
          <?php
           foreach(range('a','z') as $letter)
            {
               echo "<li>$letter</li>";
            }
          ?>
        </ul>
        <div class="clear"></div>
    </div>
      <div id="filters"><span>Selected classes:</span><ul></ul></div>
 </div>
    </body>
</html>