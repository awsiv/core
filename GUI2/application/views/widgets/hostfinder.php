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
        <form id="searchhost" action="<?php echo site_url('widget/search_by_hostname');?>">
            <span class="search">
            <input type="text" name="search" value="Search by hostname"/>
            <input class="hidden" type="submit" value="" />
            </span>
        </form>
    
        <ul id="searchby">
            <li id="search_hostname">Host Name</li>
            <li id="search_ip">Ip Address</li>
            <li id="search_class">Class</li>
        </ul>
        <div id="searchresult">
            <ul class="result">
                <?php
                foreach($hostlist as $host)
                {
                   echo "<li><a href=".site_url('welcome/host')."/".$host->key." title=".$host->key.">$host->id</a></li>";
                }
               ?>
            </ul>
        </div>
        <ul id="aplhaPaging" class="alphasearch">
          <?php
           foreach(range('A','Z') as $letter)
            {
               echo "<li>$letter</li>";
            }
          ?>
        </ul>
        <div class="clear"></div>
        <div id="filters">
            <ul class="classfilters">

            </ul>
        </div>
 </div>
    </body>
</html>