<div id="YEARLY_placeholder_<?php echo $observable; ?>" style="height: 500px; width: 70%; "></div>
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
        }

        
    };

   

    var plot = $.plot($("#YEARLY_placeholder_<?php echo $observable; ?>"),
    [  
        { data:magnifiedViewdata, magnifiedView: {show: true, lineWidth: 1}},
        { data:lineSeries1,lines: {show:true},color:"rgb(255, 153, 0)"},
        { data:lineSeries2,lines: {show: true, lineWidth: 1},color:"rgb(0, 204, 0)"}
    ]
    ,options);

    //]]>
</script>

