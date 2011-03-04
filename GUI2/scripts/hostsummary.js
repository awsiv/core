function hostsummary(redhosts,yellowhosts,greenhosts,total){
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
  var json = [
      //"root" node is invisible
      {
        "id": "node0",
        "name": "",
        "data": {
          "$type": "none"
        },
        "adjacencies": [
            {
              "nodeTo": "node1",
              "data": {
                '$type': 'none'
              }
            }, {
              "nodeTo": "node2",
              "data": {
                '$type': 'none'
              }
            }, {
              "nodeTo": "node3",
              "data": {
                '$type': 'none'
              }
            }
        ]
      }, {
        "id": "node1",
        "name": "Red Hosts",
        "data": {
          "$angularWidth": (360/total)*redhosts,
          "$color": "#dd3333",
          "$height": 70,
          "numbers":redhosts,
          "type":"red"
        },
        "adjacencies": [
            {
              "nodeTo": "node2",
              "data": {
                "$color": "#ddaacc",
                "$lineWidth": 4
              }
            }, {
              "nodeTo": "node3",
              "data": {
                "$color": "#ccffdd",
                "$lineWidth": 4
              }
            }
        ]
      }, {
        "id": "node2",
        "name": "Yellow Hosts",
        "data": {
          "$angularWidth": (360/total)*yellowhosts,
          "$color": "#FFFF00",
          "$height": 73,
          "numbers":yellowhosts,
          "type":"yellow"
        },
        "adjacencies": [
            "node1", "node3"
        ]
      }, {
        "id": "node3",
        "name": "Green hosts",
        "data": {
          "$angularWidth": (360/total)*greenhosts,
          "$color": "#bf0",
          "$height": 75,
          "numbers":greenhosts,
          "type":"green"
        },
        "adjacencies": [
            "node1", "node2"
        ]
      }
  ];
  //end
  //init Sunburst
  var sb = new $jit.Sunburst({
    //id container for the visualization
    injectInto: 'hostsummary',
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
         var data = node.data;
         if("numbers" in data) {
           html += "<strong>" + data.numbers + "</strong> hosts";
         }
           html += "<br /><b>Out of</b> " + total + " hosts";
         tip.innerHTML = html;
       }
     },
    Events: {
      enable: true,
      type: 'Native',
      //List node connections onClick
      onClick: function(node, eventInfo, e){
        if (!node) return;
        var html = "<h4>" + node.name + " connections</h4><ul><li>", ans = [];
       // alert(node.data.type);
        window.location.replace("/welcome/hosts/"+node.data.type);
        /*node.eachAdjacency(function(adj){
          // if on the same level i.e siblings
            if (adj.nodeTo._depth == node._depth) {
              ans.push(adj.nodeTo.name);
            }
          });
        $jit.id('inner-details').innerHTML = html + ans.join("</li><li>") + "</li></ul>";*/
      }
    },
    levelDistance: 40,
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