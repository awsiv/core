<div class="graphunit"><?php echo urldecode($units); ?></div>
<div id="placeholder-weekly_<?php echo $observable; ?>" style="height: 300px; width: 95%; "></div>
<div>
    <h3>Legend</h3>
    <ul>
        <li><span style="display:inline-block;width:20px;color:rgb(255, 153, 0);font-size: 15px; font-weight: bold;text-align: center;">&mdash;</span> Last measured value</li>
        <li><span style="display:inline-block;width:20px;color:rgb(0, 204, 0);font-size: 15px; font-weight: bold;text-align: center;">&mdash;</span> Average measured value</li>
        <li><span style="display:inline-block;width:20px;color:red;font-size: 15px; font-weight: bold;text-align: center;">|</span> std dev above average </li>   
        <li><span style="display:inline-block;width:20px;color:rgb(0, 204, 0);font-size: 15px; font-weight: bold;text-align: center;">|</span> std dev below average</li>
    </ul>

    <h3>Analysis</h3>
    <ul>
        <?php foreach ((array) $graphDetails as $details) { ?>
            <li><?php echo $details; ?></li>   
        <?php } ?>
    </ul>
</div>
<script language="javascript" type="text/javascript">
    //<![CDATA[
    (function() {
        
        // setup background areas
        var actualDate =parseInt("<?php echo $graphLastUpdated; ?>",10);
        var d = new Date();
        d.setTime( actualDate  * 1000 );
        // getday sunday is 0 .. so we need to align this with the current data
        var weekday=new Array();
        weekday[0]=144;
        weekday[1]=0;
        weekday[2]=24;
        weekday[3]=48;
        weekday[4]=72;
        weekday[5]=96;
        weekday[6]=120;
        
        
        
        
        var alignX = weekday[d.getDay()] + d.getHours() + d.getMinutes()/60;
        var markings = [
          
            { color: '#000', lineWidth: 1, xaxis: { from: alignX, to: alignX } }
        ];
    
        var magnifiedViewdata = <?php echo ($graphdata); ?>;
        var lineSeries1 = <?php echo ($graphdatalineseries1); ?>;
        var lineSeries2 = <?php echo ($graphdatalineseries2); ?>;
        var options = { series: {
                magnifiedView: {
                    active: true,
                    lastUpdated : "<?php echo $graphLastUpdated; ?>"
                }
            }
            ,grid:   { hoverable: true, clickable: true,axisMargin: 15,labelMargin: 15,markings: markings},
            yaxis: {
                min: <?php echo $graphdatamin; ?>,
                max: <?php echo $graphdatamax; ?>
            },

            xaxis: {
                max:168,
                tickSize:24,
                ticks: [[0, "mon"], [24, "tue"], [48, "wed"],[72, "thurs"],[96, "fri"],[120,"sat"],[144,"sun"]]

            }
        };

   
   
   
        var placeholder = $("#placeholder-weekly_<?php echo $observable; ?>");
        var plot = $.plot(placeholder,
        [
            { data:magnifiedViewdata, magnifiedView: {show: true, lineWidth: 1}},
            { data:lineSeries1,lines: {show:true},color:"rgb(255, 153, 0)"},
            { data:lineSeries2,lines: {show: true, lineWidth: 1},color:"rgb(0, 204, 0)"}
        ]
        ,options);
        drawDateLabel();
        placeholder.resize(drawDateLabel);

        function drawDateLabel() {
              // add labels

        var o;


        o = plot.pointOffset({ x: alignX, y: <?php echo $graphdatamax; ?>});
        // we just append it to the placeholder which Flot already uses
        // for positioning
        //var labelDate = d.toGMTString();
        var labelDate = common.time.format(common.unixTimeToJavascriptTime(actualDate));
        placeholder.find('.current-datelabel').remove();
        placeholder.append('<div class="current-datelabel" style="position:absolute;left:' + (o.left + 14) + 'px;top:' + (o.top+5) + 'px;color:#666;font-size:smaller">'+labelDate+'</div>');
        // draw a little arrow on top of the last label to demonstrate
        // canvas drawing
        var ctx = plot.getCanvas().getContext("2d");
        ctx.beginPath();
        o.left += 1;
        var ttop = o.top+20;
        ctx.moveTo(o.left, ttop-5);
        ctx.lineTo(o.left, ttop - 10);
        ctx.lineTo(o.left + 10, ttop-5);
        ctx.lineTo(o.left, ttop);
        ctx.fillStyle = "#000";
        ctx.fill();
        }




        $(placeholder).bind("plothover", function (event, pos, item) {
            if (item) {
                if (previousPoint != item.datapoint) {
                    previousPoint = item.datapoint;

                    $("#tooltip").remove();
                    var x = item.datapoint[0].toFixed(2),
                    y = item.datapoint[1].toFixed(2);

                    showTooltip(item.pageX, item.pageY, y);
                }
            }
            else {
                $("#tooltip").remove();
                previousPoint = null;
            }
        });
        function showTooltip(x, y, contents) {
                          

            $('<div id="tooltip">' + contents + '</div>').css( {
                position: 'absolute',
                display: 'none',
                top: y + 5,
                left: x + 5,
                border: '1px solid #fdd',
                padding: '10px',
                color: '#000',
                'background-color': '#fee',
                opacity: 0.80
            }).appendTo("body").fadeIn(200);
        }



    })();
    //]]>
</script>

