<div id="placeholder-magview_<?php echo $observable; ?>" style="height: 300px; width: 95%; "></div>
<script language="javascript" type="text/javascript">
    //<![CDATA[    
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
            min: <?php echo  $graphdatamin; ?>,
            max: <?php echo $graphdatamax; ?>
        },

        xaxis: {
            max:48,

            tickSize:12,
            //ticks: [[0, "t-4"], [12, "t-3"], [24, "t-2"],[36, "t-1"],[48, "now"]],

            tickFormatter: function (val, axis) {

                var subtract = (4-(val/12))*60*60*1000;

                var actualDate = parseInt(options.series.magnifiedView.lastUpdated,10);
                var displayDate = actualDate -subtract;
                var d = new Date(displayDate);
                return d.getHours() + ' : ' + d.getMinutes();


            }
        }
    };

   

    var plot = $.plot($("#placeholder-magview_<?php echo $observable; ?>"),
    [  
        { data:magnifiedViewdata, magnifiedView: {show: true, lineWidth: 1}},
        { data:lineSeries1,lines: {show:true},color:"rgb(255, 153, 0)"},
        { data:lineSeries2,lines: {show: true, lineWidth: 1},color:"rgb(0, 204, 0)"}
    ]
    ,options);

    //]]>
</script>

