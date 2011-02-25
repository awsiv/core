<div id="infovis" style="height: 600px;margin:5px; width: 90%; "></div>
<div id="log"></div>
<script id="source" language="javascript" type="text/javascript">
    //<![CDATA[
    
    jQuery.noConflict();
    // Use jQuery via jQuery(...)
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
    });
    var Log = {
        elem: false,
        write: function(text){
            if (!this.elem)
                this.elem = document.getElementById('log');
            this.elem.innerHTML = text;
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
                lineWidth: 0.4
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
                    //count connections
                    var count = 0;
                    node.eachAdjacency(function() { count++; });
                    //display node info in tooltip
                    tip.innerHTML = "<div class=\"tip-title\">" + node.name + "<\/div>"
                        + "<div class=\"tip-text\"><b>connections:<\/b> " + count + "<\/div>";
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
                    console.log(node);
                    if (node.data.link) {
                        //window.location.href = (node.data.link);
                    }
                }
            },
            //Number of iterations for the FD algorithm
            iterations: 200,
            //Edge length
            levelDistance: 130,
            // Add text to the labels. This method is only triggered
            // on label creation and only for DOM labels (not native canvas ones).
            onCreateLabel: function(domElement, node){
                domElement.innerHTML = node.name;
                var style = domElement.style;
                style.fontSize = "0.9em";
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


    //]]>
</script>  
