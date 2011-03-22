<div id="YEARLY_placeholder_<?php echo $observable; ?>" style="height: 500px; width: 70%; "></div>
<script language="javascript" type="text/javascript">
    //<![CDATA[
    (function() {
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
            max:156,
            tickSize:4,
            ticks: [[0, "-3 years"], [52, "-2 years"], [103, "-1 years"],[155,'<?php echo date('D F d Y',$graphLastUpdated); ?>']]

        }

        
        };

   

        var plot = $.plot($("#YEARLY_placeholder_<?php echo $observable; ?>"),
        [
            { data:magnifiedViewdata, magnifiedView: {show: true, lineWidth: 1}},
            { data:lineSeries1,lines: {show:true},color:"rgb(255, 153, 0)"},
            { data:lineSeries2,lines: {show: true, lineWidth: 1},color:"rgb(0, 204, 0)"}
        ]
        ,options);




        $("#YEARLY_placeholder_<?php echo $observable; ?>").bind("plothover", function (event, pos, item) {
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
                padding: '2px',
                color: '#000',
                'background-color': '#fee',
                opacity: 0.80
            }).appendTo("body").fadeIn(200);
        }


    })();
    //]]>
</script>

