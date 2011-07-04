
<div id="body" class="">
        <div class="grid_9 outerdiv">
            <div id="overall" class="innerdiv">
              

                <div id="pie-charts">

                    <div id="business-value-pie-chart-container" class="grid_4">
                        <p class="title">Business Value</p>
                        <div id="business-value-pie-chart" style="height: 120px;width:300px;">
                        </div>
                    </div>
                    <div id="compliance-value-pie-chart-container" class="grid_4 push_1" style="margin-bottom: 15px">
                        <p class="title">Host Status</p>
                        <div id="compliance-now-pie-chart" style="height: 120px; width:300px;">
                        </div>
                    </div>
                </div>

            </div>
            <div id="compliance" class="innerdiv">
                <p class="title">Compliance Summary</p>
                <div id="environment-list">
                    <ul>
                        <li><!-- chrome fix --></li>
                        <li><span class="front"></span><a class ="updateComplianceGraph" href="/welcome/getJsonComplianceSummary/" title="">Default</a><span class="rear"></span></li>
                        <?php foreach ($envList as $key => $env) {
                            ?>
                            <li><span class="front"></span><a class ="updateComplianceGraph" href="/welcome/getJsonComplianceSummary/<?php echo $env; ?>" title="environment_<?php echo $env; ?>"><?php echo $env; ?></a><span class="rear"></span></li>
                        <?php } ?>
                    </ul>
                </div>
                <div id="compliancemeter" style="padding:5px;">
                    <div id="compliance_summary_graph" style="height: 200px; width:98%;"></div>
                    <div class="clear"></div>
                </div>
            </div>
        </div>
        </div>
        <div class="grid_3 outerdiv">

            <div id="goals" class="innerdiv statusgoal">
                <p class="title">Services/goals</p>
                <ul>
                    <?php
                    foreach ((array) $goals as $goal) {
                        $words = explode("_", $goal->name);
                        echo "<li><span class=\"goal\">$words[0] $words[1]</span> - <span>$goal->desc</span><span class=\"check\"></span></li>";
                    }
                    ?>
                </ul>
            </div>
        </div>
</div>
<script type="text/javascript">
    // for summary compliance graph

    var d1 = <?php echo $compliance_summary['graphSeries']['keptseries']; ?>;
 
    var d2 = <?php echo $compliance_summary['graphSeries']['repairedseries']; ?>;
 
    var d3 = <?php echo $compliance_summary['graphSeries']['notkeptseries']; ?>;
    var d4 = <?php echo $compliance_summary['graphSeries']['nodataseries']; ?>;
    
    
    
    var countTrack = <?php echo $compliance_summary['graphSeries']['count']; ?>;
    var graphEnvironment = '';
    
    
    var startData = [
        {   //first series
            label:"kept",
            data: d1
         
        },
        {   //second series
            label:"repaired",
            data: d2
            
        },
        {   //3rd series
            label:"notkept",
            data: d3
           
        },  
        {   //4th series
            label:"nodata",
            data: d4
           
        }
    ];
    
    
    
    var options = {
        series: {
            stack: false,             
            bars: { show: true,lineWidth:0, barWidth: 18000000,fill:1,align:'center' },
            lines: {show:false,fill:false,steps: false,lineWidth:0},
            shadowSize:  0
        },
        legend: {
            show: false
        },
        xaxis: {
            color: '#FFF',
            mode: "time",
            timeformat: "%b-%d",
            minTickSize: [1, "day"]
        },
        yaxis: {
            show: false,
            max:110
        },
            
        grid: {hoverable: true, clickable: true },
        colors: ["#6c9053", "#cfb93f", "#D43030","#000000"]
    };
    var plot = $.plot($("#compliance_summary_graph"), startData,options );
        
       
 
    
    
    $("#compliance_summary_graph").bind("plothover", function (event, pos, item) {
        $("#x").text(pos.x.toFixed(2));
        $("#y").text(pos.y.toFixed(2));
 
        
        if (item) {
            previousPoint = item.dataIndex;
                    
            $("#tooltip").remove();
            var x = item.datapoint[0].toFixed(2),
            y = item.datapoint[1].toFixed(2);
            var actualvalue = item.series.data[item.dataIndex][1];
            var startTime = (item.datapoint[0]);
            
            var startDate = new Date(startTime );
            var start = startDate.getHours() + ':00';
            var stopDate = new Date(startDate);
            stopDate.setHours(stopDate.getHours() + 6);
            var stop =stopDate.getHours() + ':00';
            var key =  startTime /1000;
                   
            var c = countTrack[key];
                     
            var tooltip = item.series.label + ": "+ actualvalue + '<br /> Local time : ' + start + '-' + stop
                + ' <br />Count :: ' + c;
                   
            ;
            showTooltip(item.pageX, item.pageY,
            tooltip);
                
        }
        else {
            $("#tooltip").remove();
            previousPoint = null;            
        }
        
    });
    
    
    $("#compliance_summary_graph").bind("plotclick", function (event, pos, item) {
        if (item) {
            var element = $('<a href="/widget/summaryCompliance/start/'+ (item.datapoint[0] /1000)+ '/env/' + graphEnvironment+ '" title="overview" />');
            element.ajaxyDialog().ajaxyDialog("open");
        }
    });
    
   
    
    $('.updateComplianceGraph').click(function(){
        var url = $(this).attr('href');
        var env = $(this).attr('title');
        $(this).parent().siblings(".selected").removeClass("selected").find('.tippointer').remove();
        $(this).parent().addClass('selected');
        $('<div class="tippointer">').css({'margin-top':5,'left':$(this).parent().width()/2 -6}).appendTo($(this).parent());

        $.getJSON(url, function(data) {

            var json = data.graphdata;
            // update count and start track as well
            countTrack =  data.countData;
           
            //update the graph environment variable
            graphEnvironment = env;
              
            $.plot($("#compliance_summary_graph"), json,options );
        });
        return false;
    });
    
   
    var businessValueData =  [
        { label: "kept",  data: <?php echo $businessValuePie['kept']; ?>,color:"#90A316"},
        { label: "notkept",  data: <?php echo $businessValuePie['notkept']; ?>,color:"#C33D54"},
        { label: "repaired",  data: <?php echo $businessValuePie['repaired']; ?>,color:"#BFAA54"},
        { label: "No Data",  data: <?php echo $businessValuePie['nodata']; ?>,color:"#000000"}
    ];

    $.plot($("#business-value-pie-chart"), businessValueData,
    {
        series: {
            pie: {
                show: true,
                radius: 0.7,
                tilt: 0.3,
                label: {
                    show: false
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


    var complianceNowData =  [
        { label: "green",  data: <?php echo $greenhost; ?>,color:"#90A316"},
        { label: "yellow",  data: <?php echo $yellowhost; ?>,color:"#BFAA54"},
        { label: "red",  data: <?php echo $redhost; ?>,color:"#D43030"}
    ];
    $.plot($("#compliance-now-pie-chart"), complianceNowData,
    {
        series: {
            pie: {
                show: true,
                radius: 0.7,
                tilt: 0.3,
                label: {
                    show: false
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

    
    $("#business-value-pie-chart").bind("plothover",{'unit':'mu'},pieHover);
    $("#compliance-now-pie-chart").bind("plothover",{'unit':'host'}, pieHover);
    function pieHover(event, pos, obj)
    {
        if (!obj) {
            $("#tooltip").remove();
            return;
        }
        percent = parseFloat(obj.series.percent).toFixed(2);
        
        var index = obj.dataIndex;
        var value = obj.series.data[index][1];
        var content = '<span>'+obj.series.label+' ('+percent+'%) <br /> Value:: '+value+' '+event.data.unit+' '+ '</span>';
        $("#tooltip").remove();
        showTooltip(pos.pageX, pos.pageY, content);
    }




    function showTooltip(x, y, contents) {


        $('<div id="tooltip">' + contents + '</div>').css( {
            position: 'absolute',
            display: 'none',
            top: y + 5,
            left: x + 5,
            border: '1px solid #000000',
            padding: '2px',
            color: '#000',
            'background-color': '#cccccc',
            opacity: 1
        }).appendTo("body").fadeIn(200);
    }




</script>

