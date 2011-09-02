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

     <div id="searchby">
        <ul>
            <li id="search_hostname" class="selected">Host Name</li>
            <li id="search_ip">Ip Address</li>
            <li id="search_class">Class</li>
        </ul>
     </div>
            
        <div id="searchresult">
            <ul class="result">
                <?php
                if(isset($hostlist)){
                    foreach($hostlist as $host)
                    {
                       echo "<li><a href=".site_url('welcome/host')."/".$host['key']." title=".$host['key'].">".$host['id']."</a></li>";
                    }
                }else{
                     echo "<li>$error</li>";
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