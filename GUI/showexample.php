<?php
include 'header.php';
$example = $_GET['example'];
$docroot = cfpr_docroot();
?>
    <div id="nav">
             <ul class="grid_10">
              <li><a href="index.php">SUMMARY</a></li>
              <li><a href="helm.php">PLANNING</a></li>
              <li><a href="status.php" class="current">STATUS</a></li>
              <li><a href="knowledge.php">LIBRARY</a></li>
             </ul>
             <span id="status" class="grid_2 alignright">
             status
             </span>
             <div class="clearleft"></div>
        </div>
      <div id=tabpane>
       <pre>
		  <?php
          include("$docroot/docs/$example");
          ?>
         </pre>
      </div>
<?php
include 'footer.php';
?>