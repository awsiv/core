function reportcontrol2(json,basis)
{
     var ua = navigator.userAgent,
      iStuff = ua.match(/iPhone/i) || ua.match(/iPad/i),
      typeOfCanvas = typeof HTMLCanvasElement,
      nativeCanvasSupport = (typeOfCanvas == 'object' || typeOfCanvas == 'function'),
      textSupport = nativeCanvasSupport
        && (typeof document.createElement('canvas').getContext('2d').fillText == 'function');
  //I'm setting this based on the fact that ExCanvas provides text support for IE
  //and that as of today iPhone/iPad current text support is lame
  var labelType = (!nativeCanvasSupport || (textSupport && !iStuff))? 'Native' : 'HTML';
  var nativeTextSupport = labelType == 'Native';
  var useGradients = nativeCanvasSupport;
  var animate = !(iStuff || !nativeCanvasSupport);
  
var reports = new $jit.Sunburst({
     //id container for the visualization
     injectInto: 'controltest',
     'width': 600,
     'height':600 ,
     //Distance between levels
     levelDistance: 90,
     //Change node and edge styles such as
     //color, width and dimensions.
     Node: {
       overridable: true,
       type: useGradients? 'gradient-multipie' : 'multipie'
     },
     //Select canvas labels
     //'HTML', 'SVG' and 'Native' are possible options
     Label: {
       type: labelType
     },
     //Change styles when hovering and clicking nodes
     NodeStyles: {
       enable: true,
       type: 'Native',
       stylesClick: {
         'color': '#33dddd'
       },
       stylesHover:false
     },
     //Add tooltips
     Tips: {
       enable: true,
       onShow: function(tip, node) {
         var html = "<div class=\"tip-title\">" + node.name + "</div>";
         var data = node.data;
         if("days" in data) {
           html += "<b>Last modified:</b> " + data.days + " days ago";
         }
         if("size" in data) {
           html += "<br /><b>File size:</b> " + Math.round(data.size / 1024) + "KB";
         }
         tip.innerHTML = html;
       }
     },
     //implement event handlers
     Events: {
       enable: true,
       onClick: function(node) {
         if(!node) return;
         reports.tips.hide();
         //rotate
         reports.rotate(node, animate? 'animate' : 'replot', {
           duration: 1000,
           transition: $jit.Trans.Quart.easeInOut

       });
       },
       onMouseEnter:function(node){
           node.setData('color','#dd3333');
        node.eachSubnode(function(node) {
                  node.setData('color','#dd3333');
                  console.log(node.name +' ' +node.getData('color'));
                });
        reports.refresh();
       },
       onMouseLeave:function(node){
            node.setData('color','#3333dd');
            node.eachSubnode(function(node) {
                  //alert(node.name);
                  //alert(node.getData('color'));
                  node.setData('color','#3333dd');
                  console.log(node.name +' ' +node.getData('color'));
                 
            });
         reports.refresh();
       }
     },
     // Only used when Label type is 'HTML' or 'SVG'
     // Add text to the labels.
     // This method is only triggered on label creation
     onCreateLabel: function(domElement, node){
       var labels = sb.config.Label.type,
           aw = node.getData('angularWidth');
       if (labels === 'HTML' && (node._depth < 2 || aw > 2000)) {
         domElement.innerHTML = node.name;
       } else if (labels === 'SVG' && (node._depth < 2 || aw > 2000)) {
         domElement.firstChild.appendChild(document.createTextNode(node.name));
       }
     },
     // Only used when Label type is 'HTML' or 'SVG'
     // Change node styles when labels are placed
     // or moved.
     onPlaceLabel: function(domElement, node){
       var labels = sb.config.Label.type;
       if (labels === 'SVG') {
         var fch = domElement.firstChild;
         var style = fch.style;
         style.display = '';
         style.cursor = 'pointer';
         style.fontSize = "0.8em";
         fch.setAttribute('fill', "#fff");
       } else if (labels === 'HTML') {
         var style = domElement.style;
         style.display = '';
         style.cursor = 'pointer';
         style.fontSize = "0.8em";
         style.color = "#ddd";
         var left = parseInt(style.left);
         var w = domElement.offsetWidth;
         style.left = (left - w / 2) + 'px';
       }
     }
});
 //load JSON data.
 reports.loadJSON(json);
 //compute positions and plot.
 reports.refresh();
}
