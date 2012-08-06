<div id="body" class="outerdiv">
    <div class="engineeringContainer">
        <div class="grid_4 alpha">
            <div class="engineeringNavigationContainer">
                <div id="astrolabe"  class="astrolabe">
                    <div class="menu" style="text-align: center;margin: auto;"><span style="color: #FFFFFF;font-weight: bold">See also</span></div>
                    <?php
                    if ($showLeads)
                    {
                        ?>
                        <div id="scrollable_list">
                            <?php
                            require_once('leads.php');
                            ?>
                        </div>
                    <?php } ?>
                </div>
                <div class="clear"></div>
            </div>
            <div class="clear"></div>
        </div>

        <div id="darktabs" style="height: 480px;" >
            <div class="grid_8 omega" >
                <div  id="tabs-layout">
                    <ul>
                        <li><a href="#tabs-1"><?php echo $this->lang->line('knowledge_tab_map'); ?></a></li>
                        <?php
                        if ($showSameContext)
                        {
                            ?><li><a href="#tabs-4"><?php echo $this->lang->line('knowledge_tab_same_context'); ?></a></li> <?php } ?>
                        <?php
                        if ($showSubTopics)
                        {
                            ?><li><a href="#tabs-5"><?php echo $this->lang->line('knowledge_tab_subtopic'); ?></a></li> <?php } ?>
                        <?php
                        if (isset($showStory) && $showStory)
                        {
                            ?><li><a href="#tabs-6">Stories</a></li> <?php } ?>
                    </ul>

                    <div id="tabs-1" class="ui-corner-all">
                        <div class="panel">
                            <div class="panelhead"><?php echo $this->lang->line('knowledge_copernicus'); ?>: <b><?php echo $topicDetail['topic']; ?></b> in the context of <b><?php echo $topicDetail['context']; ?></b></div>
                            <div class="panelcontent"  style="height:350px;min-height: 350px;">
                                <div id="infovis" style="width:90%;height:350px;"></div>
                                <div id="log"></div>

                            </div>
                        </div>
                    </div>
                    <?php
                    if ($showSameContext)
                    {
                        ?>
                        <div id="tabs-4" class="ui-corner-all" style="overflow: auto;" >
                            <?php
                            require_once('subcategory.php');
                            ?>

                        </div>
                    <?php } ?>

                    <?php
                    if ($showSubTopics)
                    {
                        ?>
                        <div id="tabs-5" class="ui-corner-all">

                            <?php
                            require_once('category.php');
                            ?>
                        </div>
                    <?php } ?>

                    <?php
                    if (isset($showStory) && $showStory)
                    {
                        ?>
                        <div id="tabs-6" class="ui-corner-all">
                            <div id="story" style="position:relative"></div>
                        </div>
                    <?php } ?>

                </div>


                <div class="clear"></div>

                <div class="innerdiv grid_8 topicHits" style="border:1px solid #AAAAAA;">
                    <?php
                    if ($showTopicHits)
                    {
                        ?>
                        <div>
                            <?php
                            require_once('topics.php');
                            ?>
                        </div>
                    <?php }
                    else
                    { ?>
                    <div>
                        No topics to display
                    </div>
<?php } ?>
                </div>

            </div>



        </div>
        <div class="clear"></div>
    </div>



    <div class="clear"></div>
</div>
<div class="clear"></div>





<script id="source" language="javascript" type="text/javascript">








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

        $( "#tabs-layout" ).tabs(
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



        $('#tabs-layout div.ui-tabs-panel').height(function() {
            return $('#darktabs').height()
                - $('#darktabs #tabs ul.ui-tabs-nav').outerHeight(true)
                - ($('#tabs-layout').outerHeight(true) - $('#tabs-layout').height())
            // visible is important here, sine height of an invisible panel is 0
                - ($('#tabs-layout div.ui-tabs-panel:visible').outerHeight(true)
                - $('#tabs-layout div.ui-tabs-panel:visible').height());
        });

        $('#scrollable_list').height($('#darktabs').height() +  $('.topicHits').height() + 10);
        $('#scrollable_list').jScrollPane({
            showArrows: true,
            autoReinitialise: true,
            verticalArrowPositions: 'split'
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
                    dim = 18;
                    color = node.getData('color');

                    var radgrad = ctx.createRadialGradient(pos.x,pos.y,1,pos.x,pos.y,dim);
                    radgrad.addColorStop(0, '#FFFFFF');
                    radgrad.addColorStop(1, color);
                    ctx.fillStyle = radgrad;

                    this.nodeHelper.circle.render('fill', pos, dim, canvas)
                }            ,
                'contains': function(node, pos){
                    var npos = node.pos.getc(true),
                    dim = 20;
                    return this.nodeHelper.circle.contains(npos, pos, dim);

                }
            }});



        // init ForceDirected
        var fd = new $jit.ForceDirected({
            //id of the visualization container
            injectInto: 'infovis',
            height:350,
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
            levelDistance: 90,
            // Add text to the labels. This method is only triggered
            // on label creation and only for DOM labels (not native canvas ones).
            onCreateLabel: function(domElement, node){
                var label = node.name.substring(0,10);
                if (node.name.length>10) label += '....';
                domElement.innerHTML = label;
                var style = domElement.style;
                style.fontSize = "0.6em";
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
            property: ['end', 'start', 'current'],
            onStep: function(perc){
                Log.write(perc + '% loaded...');
            },
            onComplete: function(){
                Log.write('done');
                fd.plot();
            }
        });
        // end

    }
</script>