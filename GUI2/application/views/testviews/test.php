<html>
    <head>
        <title>filtertest</title>
        <script type="text/javascript" src="<?php echo get_scriptdir()?>jquery-1.4.2.min.js"></script>
    </head>
</html>
<body>
  <div id="tagCloud">
      <h2>Tag Cloud</h2>
  </div>
    <ul id="filterlist">
        
    </ul>

   <script type="text/javascript">
    $(function() {
       //get tag feed
       var json=  {
            "host": [
                {
                    "name": "cfengine",
                    "hostkey": "12345"
                }
            ] ,
            "classes": [
                {
                    "name": "windows",
                    "count": "12"
                },
                 {
                    "name": "linux",
                    "count": "12"
                },
                 {
                    "name": "solaris",
                    "count": "12"
                }

            ]
        };

       $("<ul>").attr("id", "tagList").appendTo("#tagCloud");
          $.each(json.classes, function(i, val) {
              var li = $("<li>");
              $("<a>").text(val.name).attr({title:"See all pages tagged with " + val.tag, href:"http://localhost/tags/" + val.tag + ".html"}).appendTo(li);
              li.appendTo("#tagList");
             });

       $('#tagCloud').find('li').find('a').live('click',function(event){
           event.preventDefault()
           console.log($(this).text()) ;
           var li = $("<li>");
           li.text($(this).text()).attr({title:"remove class"}).appendTo("#filterlist");
           $("<a>").text("x").appendTo(li);  
       });

       $("#filterlist").find('li').find('a').live('click',function(event){
            event.preventDefault()
            $(this).parent().remove();
       });
       
      });
    </script>
</body>

   