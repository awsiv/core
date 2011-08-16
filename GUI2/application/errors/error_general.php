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
            
            .message h1 {
                font-weight: bold;
                color:red;
            }
            
        </style>
    </head>
    <body>
        <div  class="container_12" style="margin-top:10%">

            <div id="centeredDiv">
                <div class="grid_7">
                    <img src="/themes/default/images/error_cf.png" />
                    <img src="/themes/default/images/error_agent.png" alt="error" style="float: right;" />
                    <div style="color:#4d466b;font-size: 14px;font-weight: bold;" class="message">
                        <p><?php echo $heading; ?></p>
                        <p><?php echo $message; ?></p>                       
                    </div>

                </div>
            </div>
        </div>

    </body>
</html>