<div class="graphunit"><?php echo urldecode($units); ?></div>
<div id="placeholder-magview_<?php echo $observable; ?>" style="height: 300px; width: 95%; "></div>
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
    (function(){
        var magnifiedViewdata = <?php echo ($graphdata); ?>;
        var lineSeries1 = <?php echo ($graphdatalineseries1); ?>;
        var lineSeries2 = <?php echo ($graphdatalineseries2); ?>;
        var options = { series: {
                magnifiedView: {
                    active: true,
                    lastUpdated : "<?php echo $graphLastUpdated; ?>"
                }
            }
            ,grid:   { hoverable: true, clickable: true,axisMargin: 15,labelMargin: 15},
            yaxis: {
                min: <?php echo $graphdatamin; ?>,
                max: <?php echo $graphdatamax; ?>
            },

            xaxis: {
                max:48,

                tickSize:12,
                //ticks: [[0, "t-4"], [12, "t-3"], [24, "t-2"],[36, "t-1"],[48, "now"]],

                tickFormatter: function (val, axis) {

                    var subtract = (4-(val/12))*60*60;

                    var actualDate = parseInt(options.series.magnifiedView.lastUpdated,10);
                    var displayDate = actualDate -subtract;
                    var d = new Date();
                    d.setTime( displayDate * 1000 );
                    return d.getHours() + ' : ' + d.getMinutes();


                }
            }
        };
        var plot = $.plot($("#placeholder-magview_<?php echo $observable; ?>"),
        [
            { label:"",data:magnifiedViewdata, magnifiedView: {show: true, lineWidth: 1}},
            { label:"",data:lineSeries1,lines: {show:true},color:"rgb(255, 153, 0)"},
            { label:"",data:lineSeries2,lines: {show: true, lineWidth: 1},color:"rgb(0, 204, 0)"}
        ]
        ,options);


        $("#placeholder-magview_<?php echo $observable; ?>").bind("plothover", function (event, pos, item) {
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