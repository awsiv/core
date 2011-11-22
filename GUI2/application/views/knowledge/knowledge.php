<div id="body">
    <div class="outerdiv">
        <div id="custom-tabs" style="margin:15px;">
            <ul>
                <li><a href="#tabs-1"><?php echo $this->lang->line('knowledge_tab_map'); ?></a></li>
                <?php if ($showLeads) { ?><li><a href="#tabs-2"><?php echo $this->lang->line('knowledge_tab_leads'); ?></a></li><?php } ?>
                <?php if ($showTopicHits) { ?><li><a href="#tabs-3"><?php echo $this->lang->line('knowledge_tab_references'); ?></a></li><?php } ?>
                <?php if ($showSameContext) { ?><li><a href="#tabs-4"><?php echo $this->lang->line('knowledge_tab_same_context'); ?></a></li> <?php } ?>
                <?php if ($showSubTopics) { ?><li><a href="#tabs-5"><?php echo $this->lang->line('knowledge_tab_subtopic'); ?></a></li> <?php } ?>
                <?php if ($showStory) { ?><li><a href="#tabs-6">Stories</a></li> <?php } ?>
            </ul>

            <div id="tabs-1" class="ui-corner-all">
                <div class="panel">
                    <div class="panelhead"><?php echo $this->lang->line('knowledge_copernicus'); ?>: <b><?php echo $topicDetail['topic']; ?></b> in the context of <b><?php echo $topicDetail['context']; ?></b></div>
                    <div class="panelcontent"  style="height:100%;min-height: 400px;">
                        <div id="infovis" style="width:95%;height:100%"></div>
                        <div id="log"></div>

                    </div>
                </div>
            </div>
            <?php if ($showLeads) { ?>
                <div id="tabs-2" class="ui-corner-all">
                    <?php
                    require_once('leads.php');
                    ?>
                </div>
            <?php } ?>

            <?php if ($showTopicHits) { ?>
                <div id="tabs-3" class="ui-corner-all">
                    <?php
                    require_once('topics.php');
                    ?>
                </div>
            <?php } ?>
            <?php if ($showSameContext) { ?>
                <div id="tabs-4" class="ui-corner-all">
                    <?php
                    require_once('subcategory.php');
                    ?>

                </div>
            <?php } ?>

            <?php if ($showSubTopics) { ?>
                <div id="tabs-5" class="ui-corner-all">

                    <?php
                    require_once('category.php');
                    ?>
                </div>
            <?php } ?>
            
             <?php if ($showStory) { ?>
                <div id="tabs-6" class="ui-corner-all">
                   <div id="story" style="position:relative"></div>
                </div>
            <?php } ?>
        </div>
    </div>

</div>
<div class="clear"></div>
<script id="source" language="javascript" type="text/javascript">

    //<![CDATA[
    var height = '';
    $(document).ready(function() {
        $('div#disambig').addClass('pagepanel');
        $('div#disambig').find('h2').addClass('panelhead');
        $('div#disambig').find('ul').addClass('panelcontent');       
        
        var   wheight = $(window).height(),
        cheight = wheight - 250;
        height = cheight;
    });

    var labelType, useGradients, nativeTextSupport, animate;

    jQuery(document).ready(function(){

        var ua = navigator.userAgent,
        iStuff = ua.match(/iPhone/i) || ua.match(/iPad/i),
        typeOfCanvas = typeof HTMLCanvasElement,
        nativeCanvasSupport = (typeOfCanvas == 'object' || typeOfCanvas == 'function'),
        textSupport = nativeCanvasSupport
            && (typeof document.createElement('canvas').getContext('2d').fillText == 'function');
        //I'm setting this based on the fact that ExCanvas provides text support for IE
        //and that as of today iPhone/iPad current text support is lame
        labelType = (!nativeCanvasSupport || (textSupport && !iStuff))? 'Native' : 'HTML';
        nativeTextSupport = labelType == 'Native';
        useGradients = nativeCanvasSupport;
        animate = !(iStuff || !nativeCanvasSupport);

        init();

        $( "#custom-tabs" ).tabs(
        {
            cookie: {
                // store cookie for a day, without, it would be a session cookie
                expires: 1,
                name:'ktab_cookie',
                path:'/'
            },
            
            show: function(event, ui) {
                
                // remove the cookie if it is not the reference tab
                var tabHash = '#tabs-3';
                if (ui.tab.hash != tabHash) {
                    // remove the cookie
                    $.cookie('ktab_cookie', null, { path: '/',expires: -5 });
                }
                
            }
            
            
        });
       
     

    });
    var Log = {
        elem: false,
        write: function(text){
            if (!this.elem)
                this.elem = document.getElementById('log');
            this.elem.innerHTML = text;
            if (text == 'done') this.elem.innerHTML = ''; // dont show the done label 
            this.elem.style.left = (500 - this.elem.offsetWidth / 2) + 'px';
        }
    };


    function init(){
        // init data
        var json = <?php echo $graphdata ?>;
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



        // init ForceDirected
        var fd = new $jit.ForceDirected({
            //id of the visualization container
            injectInto: 'infovis',
            height:height,
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
                dim:18
            },
            Edge: {
                overridable: true,
                color: '#000000',
                lineWidth: 0.2
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
            iterations: 200,
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
                Log.write(perc + '% loaded...');
            },
            onComplete: function(){
                Log.write('done');
                fd.animate({
                    modes: ['linear'],
                    transition: $jit.Trans.Elastic.easeOut,
                    duration: 2500
                });
            }
        });
        // end

    }
<?php if($showStory){?>
     jsPlumb.bind("ready", function() {
       var story_json=<?php echo $story?>;
       
       var container=$("#story");
       var   wheight = $(window).height(),
            cheight = wheight - 250;
            container.height(cheight);
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
            
            //for extremely small nodes
            if(val.name.length<15){
              nextleft+=val.name.length*20+linkLength;    
            }else{
              nextleft+=val.name.length*10+linkLength;  
            }
           
            
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

<?php } ?>


   
    //]]>
</script>