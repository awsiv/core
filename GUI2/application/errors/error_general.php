<html>
    <head>
        <title>Error</title>
        <link type="text/css" rel="stylesheet" href="/themes/default/css/resets.css" media="screen" />
        <link type="text/css" rel="stylesheet" href="/themes/default/css/960.css" media="screen" />
        <style type="text/css">
            p {
                padding: 5px;
            }
            html, body { margin: 0; padding: 0; }
            #centeredDiv { margin-right: auto; margin-left: auto; width: 800px; }
            
            .message h1, .message pre {
                clear: both;
            }
            
             .message .heading {
                 font-size: 1.2em;
                }
            
            
            .message h1 {
                font-weight: bold;
                color:red;
            }
            a {
                font-weight: normal;
                text-decoration: underline;
            }
            
        </style>
    </head>
    <body>
        <div  class="container_12" style="margin-top:10%">

            <div id="centeredDiv">
                <div class="grid_10">
                    <a href="/" title="Go to home page."><img src="<?php echo base_url()?>/themes/default/images/error_cf.png" /></a> 
                    <img src="<?php echo base_url()?>/themes/default/images/error_agent.png" alt="error" style="float: right;" />
                    <div style="color:#4d466b;font-size: 14px;font-weight: bold;" class="message">
                        <p class="heading"><?php echo $heading; ?></p>
                        <p class="sub-message"><?php echo $message; ?></p>                       
                    </div>

                </div>
            </div>
        </div>

    </body>
</html>