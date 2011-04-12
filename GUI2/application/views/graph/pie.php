<div id="infovis" style="height:200px;width: 60%;"></div>

<div id="graph" style="height:200px;width: 60%;"></div>

<?php
$array = json_decode($gdata);

// make data ready for pie
$kept = 0;
$notkept = 0;
$repaired = 0;

foreach ($array as $val) {


$kept+=abs($val[1]);
$notkept+=abs($val[3]);
$repaired+=abs($val[2]);


}


?>


<script type="text/javascript">
 (function() {
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
})();
   function init(){
    //init data
    var json = {
        'color': ["#90A316","#BFAA54","#C33D54"],
        'label': 'label A',
        'values': [
        {
          'label': 'kept',
          'values': Math.abs(<?php echo $kept; ?>)
        },
        {
          'label': 'repaired',
          'values': Math.abs(<?php echo $repaired; ?>)
        },
        {
          'label': 'not kept',
          'values': Math.abs(<?php echo $notkept; ?>)
        }]
    };
    
    //end
    var infovis = document.getElementById('infovis');
    //init PieChart
    pie = new $jit.PieChart({
        injectInto: 'infovis',
        animate: true,
        offset: 10,
        sliceOffset: 2,
        labelOffset: 10,
        type:'stacked:gradient',
        updateHeights:false,
        showLabels:false,
        Label: {
          size: 11,
          family: 'Arial',
          color: 'white'
        },
        Tips: {
          'enable': true,
          'onShow': function(tip, elem) {
             tip.innerHTML = "<b>" + elem.label + "</b>: " + elem.value;
          }
        }
    });

    //load JSON data.
    pie.loadJSON(json);

   
}

init();


var data = [
                { label: "kept",  data: <?php echo $kept; ?>,color:"#90A316"},
                { label: "notkept",  data: <?php echo $notkept; ?>,color:"#C33D54"},
                { label: "repaired",  data: 50000,color:"#BFAA54"}
        ];
$.plot($("#graph"), data,
{
        series: {
            pie: {
                show: true,
                radius: 0.7,
                tilt: 0.3,
                label: {
                    show: false,
                    radius: 1,
                    formatter: function(label, series){
                        return '<div style="font-size:8pt;text-align:center;padding:2px;color:white;">'+label+'<br/>'+Math.round(series.percent)+'%</div>';
                    },
                    background: { opacity: 0.8 }
                },
                highlight: {
			opacity: .5
		}
            }
        },
        legend: {
            show: false
        },grid: {
			hoverable: true,
			clickable: true
		}

});
$("#graph").bind("plothover", pieHover);
function pieHover(event, pos, obj)
{
    	if (!obj) {
             $("#tooltip").remove();
                return;
        }
        percent = parseFloat(obj.series.percent).toFixed(2);
	var content = '<span style="font-weight: bold; color: '+obj.series.color+'">'+obj.series.label+' ('+percent+'%)</span>';
        $("#tooltip").remove();
        showTooltip(pos.pageX, pos.pageY, content);
}

 


 function showTooltip(x, y, contents) {


            $('<div id="tooltip">' + contents + '</div>').css( {
                position: 'absolute',
                display: 'none',
                top: y + 5,
                left: x + 5,
                border: '1px solid #fdd',
                padding: '2px',
                color: '#000',
                'background-color': '#fee',
                opacity: 0.80
            }).appendTo("body").fadeIn(200);
        }

</script>