
<div id="placeholder-histogram_<?php echo $observable; ?>" style="height: 300px; width: 95%; "></div>
<script language="javascript" type="text/javascript">
    //<![CDATA[
    (function(){
        var viewdata = <?php echo ($graphdata); ?>;
        //avg data
        options = {grid:   { hoverable: true, clickable: true},
            series: { bars: {
                    align: 'center'            }
            },
            xaxis : {
                max:65,
                tickSize: 16,
                ticks: [[0, "-2 sig"], [16, "-sig"], [32, "avg"],[48, "+sig"],[64, "+2 sig"]]


            }
        };
        
        $.plot($("#placeholder-histogram_<?php echo $observable; ?>"), [
            { label: "", data: viewdata,  bars: { show: true,barWidth: 1,lineWidth: .1,
                    fill: true}, color: "#0000FF" },
        ], options);
    })();

    //]]>
</script>

