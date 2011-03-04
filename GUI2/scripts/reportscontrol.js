var $reportcontroldialog=$('.dialog').dialog({
		 autoOpen: false,
		 modal: true,
		 hide: 'puff',
		 resizable: false,
		 title:'Checkout',
		 buttons: {
		 'OK': function() {
			 $(this).dialog('close');
	      },
		'Cancel':function() {
	      $(this).dialog('close');
	      }
	  },
	  open: function() {

		 }
	 });
 
function reportcontrol(json,basis)
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
 
 var sb = new $jit.Sunburst({
    //id container for the visualization
    injectInto: 'reportcontrol',
    //Change node and edge styles such as
    //color, width, lineWidth and edge types
    'width': 300,
    'height':300 ,
    Node: {
      overridable: true,
      type: useGradients? 'gradient-multipie' : 'multipie'
    },
    Edge: {
      overridable: true,
      type: 'hyperline',
      lineWidth: 2,
      color: '#777'
    },
    //Draw canvas text. Can also be
    //'HTML' or 'SVG' to draw DOM labels
    Label: {
      type: nativeTextSupport? 'Native' : 'SVG'
    },
    //Add animations when hovering and clicking nodes
    NodeStyles: {
      enable: true,
      type: 'Native',
      stylesClick: {
        'color': '#33dddd'
      },
      stylesHover: {
        'color': '#55b'
      },
      duration: 700
    },
    //Add tooltips
     Tips: {
       enable: true,
       onShow: function(tip, node) {
         var html = "<div class=\"tip-title\">" + node.name + "</div>";
        
         tip.innerHTML = html;
       }
     },
    Events: {
      enable: true,
      type: 'Native',
      //List node connections onClick
      onClick: function(node, eventInfo, e){
        if (!node) return;
         sb.tips.hide();
         //rotate
         sb.rotate(node, animate? 'animate' : 'replot', {
           duration: 1000,
           transition: $jit.Trans.Quart.easeInOut

       });
       if(basis=="overall")
           {
           $('#repdialog').load('/search/index', {report: node.name}, function() {
               //$reportcontroldialog.dialog('open');
               var $dialog=$(this);
               var $closebtn=$("<a class='ui-panel-close'><span class='ui-icon ui-icon-closethick'></span></a>");
               $(this).slideDown('slow');
                $(this).find('.panelhead').append($closebtn);
                $closebtn.hover(function () {
                             $(this).addClass("ui-state-hover");
                        },
                           function () {
                             $(this).removeClass("ui-state-hover");
                         });
                 $closebtn.click(function(){
                   $dialog.fadeOut();
                 })
               });
            }
          if(basis=="host")
              {
                var html='<input type="hidden" name="report" value="'+node.name+'" />'
                $('#reportcontrol').find('form').append(html).submit();
              }
       
        /*node.eachAdjacency(function(adj){
          // if on the same level i.e siblings
            if (adj.nodeTo._depth == node._depth) {
              ans.push(adj.nodeTo.name);
            }
          });

        $jit.id('inner-details').innerHTML = html + ans.join("</li><li>") + "</li></ul>";*/
      }
    },
    levelDistance: 50,
    // Only used when Label type is 'HTML' or 'SVG'
    // Add text to the labels.
    // This method is only triggered on label creation
    onCreateLabel: function(domElement, node){
      var labels = sb.config.Label.type;
      if (labels === 'HTML') {
        domElement.innerHTML = node.name;
      } else if (labels === 'SVG') {
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
        if (node._depth <= 1) {
          style.fontSize = "0.8em";
          style.color = "#ddd";
        }
        var left = parseInt(style.left);
        var w = domElement.offsetWidth;
        style.left = (left - w / 2) + 'px';
      }
    }
  });
  // load JSON data.
  sb.loadJSON(json);
  // compute positions and plot.
  sb.refresh();
  //end
}



