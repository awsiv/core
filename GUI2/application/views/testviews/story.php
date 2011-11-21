<html>
    <head>
        <title>filtertest</title>
        <link href="<?php echo get_cssdir();?>jquery-ui-1.8.10.custom.css" rel="stylesheet" media="screen" />
        <script type="text/javascript" src="<?php echo get_scriptdir()?>jquery-1.4.2.min.js"></script>
        <style>
        .topic{
             background-color: #EEEEEF;
                border: 1px solid #346789;
                border-radius: 0.5em 0.5em 0.5em 0.5em;
                box-shadow: 2px 2px 19px #AAAAAA;
                color: black;
                font-family: helvetica;
                font-size: 0.9em;
                height: 5em;
                line-height: 5em;
                opacity: 0.8;
                padding: 0.5em;
                position: absolute;
                text-align: center;
            }
            
            .topic:hover{
                 box-shadow: 2px 2px 19px #444444;
                 opacity:0.6;
                 cursor: pointer;
            } 
            .labelClass{
                z-index: 6;
                border: 1px solid #346789;
                padding: 8px;
                opacity: 0.8;
                font-family: helvetica;
                background-color: white;
            }
        </style>
    </head>
    
   <body>
   <div id="story" style="width:500px;position:relative"></div>
   <script type="text/javascript" src="<?php echo get_scriptdir()?>jquery-ui-1.8.14.custom.min.js"></script>
   <script type="text/javascript" src="<?php echo get_scriptdir()?>jquery.jsPlumb-1.3.3-all-min.js"></script>
   <script type="text/javascript">
    jsPlumb.bind("ready", function() {

          /* var json=[
            {  
            "id": "graphnode1",   
            "name": "webservice", 
            "data": {  
              "$color": "#C74243",
               "context": "any"
            },
            "adjacencies": [
                {
                    "nodeTo": "graphnode2",
                    "nodeFrom": "graphnode1",
                    "data": {
                    "association":"implements",
                     "$direction": ["graphnode1", "graphnode2"] 
                    }  
                },
                {
                    "nodeTo": "graphnode2",
                    "nodeFrom": "graphnode3",
                    "data": {
                    "association":"is"
                    }  
                }
             ]  
          },
          {  
            "id": "graphnode2",   
            "name": "webserversadasdasdasdasdsadsd", 
            "data": {  
               "$color": "#C74243",
               "context": "application_service"
            },
            "adjacencies": []  
          },
          
          {  
            "id": "graphnode3",   
            "name": "apache", 
            "data": {  
              "$color": "#C74243",
               "context": "any"
            },
            "adjacencies": [
                {
                    "nodeTo": "graphnode4",
                    "nodeFrom": "graphnode3",
                    "data": {
                    "association":"implements",
                     "$direction": ["graphnode3", "graphnode4"] 
                    }  
                } 
             ]  
          },
          {  
            "id": "graphnode4",   
            "name": "webserver2", 
            "data": {  
               "$color": "#C74243",
               "context": "application_service"
            },
            "adjacencies": []  
          }
        ]*/
       var story_json=<?php echo $story?>;
       
       var container=$("#story");
       var common = {
	cssClass:"myCssClass"
       };
       
       jsPlumb.Defaults.DragOptions = { cursor: 'pointer', zIndex:2000 };
       jsPlumb.setMouseEventsEnabled(true);
       //draw all the nodes
       var nextleft=10,
           nexttop=20,
           direction='left';
       jQuery.each(story_json.F, function(i, val) {
           var linkLength=20;
           if(!$('#'+val.id).length>0){
            var nodeHtml=$('<div>').attr('id',val.id).addClass('topic').append('<strong>'+val.name+'</strong>');
             
            
            if(direction=='left'){
              nodeHtml.css({'left':nextleft+'px','top':nexttop+'px'}).appendTo(container);
                
            }
            if(direction=='right'){
              nodeHtml.css({'right':nextleft+'px','top':nexttop+'px'}).appendTo(container);
            }
            
            nextleft+=val.name.length*10+linkLength;
            if(nextleft>container.width() && direction=='left'){
               direction='right'
               nexttop+=nodeHtml.height()+150;
               nextleft=10;
             }
            if(nextleft<10 && direction=='right'){
               direction='left'
               nexttop+=nodeHtml.height()+150;
               nextleft=10;
             }
            //console.log(nextleft);
           }
        });
        
        //drawing the links
       jQuery.each(story_json.F, function(i, val) {
          if(val.adjacencies.length !== 0){
              jQuery.each(val.adjacencies, function(i, adj){
                  var anchors=["BottomCenter", "BottomCenter"],
                      topFrom=parseInt($('#'+adj.nodeFrom).css('top')),
                      topTo=parseInt($('#'+adj.nodeTo).css('top'))
              
                    if(topFrom > topTo){
                        anchors=["TopCenter", "BottomCenter"];
                    }
                    if(topFrom < topTo){
                          anchors=["BottomCenter", "TopCenter"]; 
                    }
                    jsPlumb.connect({
                            source:adj.nodeFrom,
                            target:adj.nodeTo,
                            anchors:anchors,
                            endpoint:[ "Dot", { radius:5 }, common ],
                            connector:[ "Bezier", { curviness:50 }, common ],
                            overlays: [
                                    [ "Arrow", { width:20, length:30, location:0.75, id:"arrow" }, common ],
                                    [ "Label", {label:adj.data.association, id:"label",cssClass:"labelClass" } ]	
                            ]
                    });
              });
              //console.log(val.adjacencies.nodeFrom+' '+val.id);
           }
        });
      }); 
   </script>
</html>
