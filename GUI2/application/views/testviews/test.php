<html>
    <head>
        <title>filtertest</title>
        <link href="<?php echo get_cssdir();?>jquery-ui-1.8.10.custom.css" rel="stylesheet" media="screen" />
        <script type="text/javascript" src="<?php echo get_scriptdir()?>jquery-1.4.2.min.js"></script>
    </head>
</html>
<body>
   <a href="#" id="selecthost">Select particular host</a>

   <script type="text/javascript" src="<?php echo get_scriptdir()?>jquery-ui-1.8.9.custom.min.js"></script>
   <script type="text/javascript" src="<?php echo get_scriptdir()?>widgets/hostfinder.js"></script>
   <script type="text/javascript">
    $('#selecthost').hostfinder();
    </script>
</body>

   