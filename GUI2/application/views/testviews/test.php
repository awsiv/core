<html>
    <head>
        <title>filtertest</title>
        <link href="<?php echo get_cssdir();?>jquery-ui-1.8.10.custom.css" rel="stylesheet" media="screen" />
        <script type="text/javascript" src="<?php echo get_scriptdir()?>jquery-1.4.2.min.js"></script>
    </head>
    
    <body>
    <div id="infovis" style="width:95%;height:100%"></div>

   <script type="text/javascript" src="<?php echo get_scriptdir()?>jit/jit.js"></script>
   <script type="text/javascript" src="<?php echo get_scriptdir()?>jit/Extras/excanvas.js"></script>
   <script type="text/javascript">
    function init(){
        // init data
       /* var json = [
             {  
            "adjacencies": [],   
            "data": {  
              "$color": "#C74243"   
              },   
            "id": "graphnode17",   
            "name": "webserver" 
            }, 
            {  
            "adjacencies": [
                {
                    "nodeTo": "graphnode17",
                    "nodeFrom": "graphnode18",
                    "data": {
                    "association":"implements",
                     "$direction": ["graphnode18", "graphnode17"] 
                    }  
                },
                 {
                    "nodeTo": "graphnode19",
                    "nodeFrom": "graphnode18",
                    "data": {
                    "association":"implements",
                     "$direction": ["graphnode18", "graphnode19"] 
                    }  
                }
                
                
              ],   
            "data": {  
              "$color": "#C74243"
            },   
            "id": "graphnode18",   
            "name": "webservice" 
          },   
          {  
           "adjacencies": [
              {  
            "nodeTo": "graphnode18",   
            "nodeFrom": "graphnode19",   
            "data": {
                "association":"implements",
                "$direction": ["graphnode19", "garphnode18"] 
                }  
             }
            ],   
        "data": {  
          "$color": "#EBB056",   
          "$type": "gradientCircle"  
        },   
        "id": "graphnode19",   
        "name": "graphnode19"  
      },  
      
       {  
           "adjacencies": [
               {  
            "nodeTo": "graphnode19",   
            "nodeFrom": "graphnode20",   
            "data": {
                "association":"implements",
                "$direction": ["graphnode19", "garphnode20"] 
                }  
             }
            ],   
        "data": {  
          "$color": "#EBB056",   
          "$type": "gradientCircle"  
        },   
        "id": "graphnode20",   
        "name": "bishwa"  
      }
      
  ];*/
        var json=[
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
                } 
             ]  
          },
          {  
            "id": "graphnode2",   
            "name": "webserver", 
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
            "name": "webserver", 
            "data": {  
               "$color": "#C74243",
               "context": "application_service"
            },
            "adjacencies": []  
          }
          
          
        ]
        // end

        //
        //Custom Gradient Node for handling the graidient circles
        // just add type as gradientCircle with the color property in the node.

        $jit.ForceDirected.Plot.NodeTypes.implement({
            'gradientCircle': {
                'render': function(node,canvas) {
                    var ctx = canvas.getCtx();
                    var pos = node.pos.getc(true),
                    dim = node.getData('dim');
                    color = node.getData('color');

                    var radgrad = ctx.createRadialGradient(pos.x,pos.y,1,pos.x,pos.y,dim);
                    radgrad.addColorStop(0, '#FFFFFF');
                    radgrad.addColorStop(1, color);
                    ctx.fillStyle = radgrad;

                    this.nodeHelper.circle.render('fill', pos, dim, canvas)
                }            ,
                'contains': function(node, pos){
                    var npos = node.pos.getc(true),
                    dim = node.getData('dim');
                    return this.nodeHelper.circle.contains(npos, pos, dim);

                }
            }});

      $jit.ForceDirected.Plot.EdgeTypes.implement({  
          'edgewithLabel': {  
            'render': function(adj, canvas) {  
              //print your custom edge to canvas  
              //plot arrow edge
               var ctx = canvas.getCtx();
                var data = adj.data;
                var pos = adj.nodeFrom.pos.getc(true);
                var posChild = adj.nodeTo.pos.getc(true);
                var inv = (adj.data.$direction && adj.data.$direction.length>1 && adj.data.$direction[0] != adj.nodeFrom.id);
               
                 if (inv) {
                   var tmp = pos;
                       pos = posChild;
                       posChild = tmp;
                  }
                //get nodes cartesian coordinates
                
                 var x = parseInt((pos.x + posChild.x) /2);
                 var xlabel = parseInt((pos.x + posChild.x - (data.association.length * 5)) /2);
                 var y = parseInt((pos.y + posChild.y ) /2);


                var vect = new $jit.Complex(x - pos.x, y - pos.y);
                vect.$scale(10 / vect.norm());
                var intermediatePoint = new $jit.Complex(x - vect.x, y - vect.y),
                    normal = new $jit.Complex(-vect.y / 2, vect.x / 2),
                    v1 = intermediatePoint.add(normal), 
                    v2 = intermediatePoint.$add(normal.$scale(-1));

                ctx.beginPath();
                ctx.moveTo(pos.x, pos.y-5);
                ctx.lineTo(posChild.x, posChild.y-5);
                ctx.stroke();
                
                ctx.beginPath();
                ctx.moveTo(pos.x, pos.y+5);
                ctx.lineTo(posChild.x, posChild.y+5);
                ctx.stroke();
                
                ctx.beginPath();
                ctx.moveTo(v1.x, v1.y);
                ctx.lineTo(v2.x, v2.y);
                ctx.lineTo(x, y);
                ctx.closePath();
                ctx.fill();
                    //this.edgeTypes.arrow.render.call(this, adj, canvas); 
                    
                    
                    //check for edge label in data
                   
                    if(data.association) {
                       //now adjust the label placement
                    var radius = this.viz.canvas.getSize();
                   // var x = parseInt((pos.x + posChild.x +radius.width) /2);
                   //var y = parseInt((pos.y + posChild.y +radius.height) /2);
                   //var radius = this.viz.canvas.getSize();
                        this.viz.canvas.getCtx().fillText(data.association, xlabel, y+15); 
                   // canvas.getCtx().fillText(data.association, x, y);
                    }
            },  
            'contains': function(adj, pos) {  
              //return true if pos is inside the arc or false otherwise  
            }  
          }  
        });


        // init ForceDirected
        var fd = new $jit.ForceDirected({
            //id of the visualization container
            injectInto: 'infovis',
            height:$(window).height(),
            //Enable zooming and panning
            //by scrolling and DnD
            Navigation: {
                enable: true,
                //Enable panning events only if we're dragging the empty
                //canvas (and not a node).
                panning: 'avoid nodes',
                zooming: 10 //zoom speed. higher is more sensible
            },
            // Change node and edge styles such as
            // color and width.
            // These properties are also set per node
            // with dollar prefixed data-properties in the
            // JSON structure.
            Node: {
                overridable: true,
                color: '#3399FF',
                dim:10,
                type:'gradientCircle'
            },
            Edge: {
                overridable: true,
                color: '#000000',
                lineWidth: 0.2,
                dim:10,
                type:'edgewithLabel'
            },
            //Native canvas text styling
            Label: {
                type: 'HTML',
                color: '#000000'

            },
            //Add Tips
            Tips: {
                enable: true,
                onShow: function(tip, node) {                  
                    //display node info in tooltip
                    tip.innerHTML = "<div class=\"tip-title\">" + node.data.fullname + "<\/div>"
                        + "<div class=\"tip-text\"><b>context:<\/b> " + node.data.context + "<\/div>";
                }
            },
            // Add node events
            Events: {
                enable: true,

                //Change cursor style when hovering a node
                onMouseEnter: function() {
                    fd.canvas.getElement().style.cursor = 'move';
                },
                onMouseLeave: function() {
                    fd.canvas.getElement().style.cursor = '';
                },
                //Update node positions when dragged
                onDragMove: function(node, eventInfo, e) {
                    var pos = eventInfo.getPos();
                    node.pos.setc(pos.x, pos.y);
                    fd.plot();
                },
                //Implement the same handler for touchscreens
                onTouchMove: function(node, eventInfo, e) {
                    $jit.util.event.stop(e); //stop default touchmove event
                    this.onDragMove(node, eventInfo, e);
                },
                //Add also a click handler to nodes
                onClick: function(node) {
                    if(!node) return;
                    if (node.data.link) {
                        window.location.href = '<?php echo site_url(); ?>'+(node.data.link);
                    }
                }
            },
            //Number of iterations for the FD algorithm
            iterations: 20,
            //Edge length
            levelDistance: 180,
            // Add text to the labels. This method is only triggered
            // on label creation and only for DOM labels (not native canvas ones).
            onCreateLabel: function(domElement, node){
                domElement.innerHTML = node.name;
                var style = domElement.style;
                style.fontSize = "0.7em";
                style.color = "#000000";
            },
            // Change node styles when DOM labels are placed
            // or moved.
            onPlaceLabel: function(domElement, node){
                var style = domElement.style;
                var left = parseInt(style.left);
                var top = parseInt(style.top);
                var w = domElement.offsetWidth;
                style.left = (left - w / 2) + 'px';
                style.top = (top -3) + 'px';
                style.display = '';
            }
        });
        // load JSON data.
        fd.loadJSON(json);
        // compute positions incrementally and animate.
        fd.computeIncremental({
            iter: 40,
            property: 'end',
            onStep: function(perc){
               // Log.write(perc + '% loaded...');
            },
            onComplete: function(){
               /* Log.write('done');*/
                fd.animate({
                    modes: ['linear'],
                    transition: $jit.Trans.Elastic.easeOut,
                    duration:0
                });
            }
        });
        // end

    }
init();
    </script>
   </body>
</html>


   