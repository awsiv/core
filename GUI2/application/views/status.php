<div class="outerdiv">
    <div class="grid_9">
        <div id="overall" class="innerdiv">
            <div id="pie-charts">

                <div id="business-value-pie-chart-container" class="grid_4">
                    <p class="title"><?php echo $this->lang->line('business-value-pie-chart-container_title'); ?></p>
                    <div id="business-value-pie-chart" style="height: 120px;width:300px;">
                    </div>
                </div>
                <div id="compliance-value-pie-chart-container" class="grid_4 push_1" style="margin-bottom: 15px">
                    <p class="title"><?php echo $this->lang->line('compliance-value-pie-chart-container_title'); ?></p>
                    <div id="compliance-now-pie-chart" style="height: 120px; width:300px;">
                    </div>
                </div>
            </div>

        </div>
        <div id="compliance" class="innerdiv">
            <p class="title"><?php echo $this->lang->line('compliance_summary_graph_title')?></p>
            <div id="environment-list">
                <ul>
                    <li><!-- chrome fix --></li>
                    <li class="selected"><span class="front"></span><a class ="updateComplianceGraph" href="<?php echo site_url(); ?>/welcome/getJsonComplianceSummary/" title="">Default</a><span class="rear"></span>
                    <div class="tippointer" style="margin-top: 5px; left: 27.5px;"></div>
                    </li>
                    <?php foreach ($envList as $key => $env) {
                        ?>
                        <li><span class="front"></span><a class ="updateComplianceGraph" href="<?php echo site_url(); ?>/welcome/getJsonComplianceSummary/<?php echo $env; ?>" title="environment_<?php echo $env; ?>"><?php echo $env; ?></a><span class="rear"></span></li>
                    <?php } ?>
                </ul>
            </div>
            <div id="compliancemeter" style="padding:5px;">
                <div id="compliance_summary_graph" style="height: 200px; width:98%;"></div>
                <div class="clear"></div>
            </div>
        </div>
    </div>
    <div class="grid_3">
         

        <div id="goals" class="innerdiv statusgoal">
            <p class="title">Services/goals</p>
            <ul>
                <?php
                $limit = 0;
                $goalsorted = array_msort($goals, array('name' => SORT_ASC), true);
                foreach ((array) $goalsorted as $goal) {
                    if ($limit == 5)
                        break;
                    $words = explode("_", $goal['name']);
                    echo "<li><span class=\"goal\">$words[0] $words[1]</span> - <span>" . $goal['desc'] . "</span><span class=\"check\"></span></li>";
                    $limit++;
                }
                ?>
            </ul>
            <p class="morebtnpane"><span class="morebtn"><?php echo anchor('welcome/goals', 'More...') ?></span></p>
        </div>
    </div>
    <div class="clear"></div>
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
            label:"Kept",
            data: d1

        },
        {   //second series
            label:"Repaired",
            data: d2

        },
        {   //3rd series
            label:"Notkept",
            data: d3

        },
        {   //4th series
            label:"No data",
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
            
        grid: {"hoverable": true, "clickable": true,"autoHighlight":true },
        colors: ["#779a62", "#F0d76a", "#cc4358","#000000"]
    };
    var plot = $.plot($("#compliance_summary_graph"), startData,options );
        
       
 
    
    var previousPoint = {point:null,sIndex:null};
    $("#compliance_summary_graph").bind("plothover", function (event, pos, item) {
        $("#x").text(pos.x.toFixed(2));
        $("#y").text(pos.y.toFixed(2));
 
        
        if (item) {
            if (previousPoint == null || previousPoint.point != item.dataIndex || previousPoint.sIndex != item.seriesIndex ) { 
                previousPoint.point = item.dataIndex;
                previousPoint.sIndex = item.seriesIndex;
                
                $(this).css('cursor','pointer');
                plot.highlight(item.series, item.datapoint,event);
                
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
                     

                var tooltip = item.series.label + " : "+ actualvalue + '<br /> Local time : ' + start + '-' + stop
                    + ' <br />Count : ' + c;
                showTooltip(item.pageX, item.pageY,tooltip);
                
            }   
        }
        else {
            $(this).css('cursor','default');
            $("#tooltip").remove();
            plot.unhighlight();
            previousPoint.point = null;
            previousPoint.sIndex = null;
        }
        
    });
    
    
    $("#compliance_summary_graph").bind("plotclick", function (event, pos, item) {
        if (item) {
            var start = (item.datapoint[0] );
            var startDate = new Date(start );
            var start = startDate.getHours() + ':00';
            var stopDate = new Date(startDate);
            stopDate.setHours(stopDate.getHours() + 6);
            var stop =stopDate.getHours() + ':00';
            var sendDate = item.datapoint[0] /1000;
            var element = $('<a href="<?php echo site_url(); ?>/widget/summaryCompliance/start/'+ sendDate + '/env/' + graphEnvironment+ '" title="Overview : Promises not kept or repaired ( '+ start + ' - ' + stop + ' )" />');
            var option = {'width':'1025','height':'445'};
            element.ajaxyDialog(option).ajaxyDialog("open");
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
        { label: "Kept",  data: <?php echo $businessValuePie['kept']; ?>,color:"#779a62"},
        { label: "Not kept",  data: <?php echo $businessValuePie['notkept']; ?>,color:"#CC4358"},
        { label: "Repaired",  data: <?php echo $businessValuePie['repaired']; ?>,color:"#F0D76A"},
        { label: "No data",  data: <?php echo $businessValuePie['nodata']; ?>,color:"#000000"}
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
        { label: "Green",  data: <?php echo $greenhost; ?>,color:"#779a62"},
        { label: "Yellow",  data: <?php echo $yellowhost; ?>,color:"#F0D76A"},
        { label: "Red",  data: <?php echo $redhost; ?>,color:"#CC4358"},
        { label: "Blue",  data: <?php echo $bluehost; ?>,color:"#476E8C"},
        { label: "Black",  data: <?php echo $blackhost; ?>,color:"#000000"}
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

    var previousPointBusinessValue = {'val':null};
    var previousPointComplianceNow = {'val':null};
    $("#business-value-pie-chart").bind("plothover",{'unit':'mu','previousPoint':previousPointBusinessValue},pieHover);
    $("#compliance-now-pie-chart").bind("plothover",{'unit':'host','previousPoint':previousPointComplianceNow}, pieHover);
    
    $($("#compliance-now-pie-chart")).bind('mouseout', function() {
        plot.unhighlight();
        $("#tooltip").remove();
        $(this).data('previous-post', -1);
    });
    var previousPost = null;
    function pieHover(event, pos, item)
    {
        
        if (item) {
            if ($(this).data('previous-post') != item.seriesIndex) {
                plot.unhighlight();
                plot.highlight(item.series, item.datapoint);
                $(this).data('previous-post', item.seriesIndex);
            } else return ;
            $("#tooltip").remove();
            var  percent = parseFloat(item.series.percent).toFixed(2);
            var index = item.dataIndex;
            var value = item.series.data[index][1];
            var content = '<span>'+item.series.label+' ('+percent+'%) <br /> Value: '+value+' '+event.data.unit+' '+ '</span>';
            showTooltip(pos.pageX, pos.pageY, content);
        } else {
            plot.unhighlight();
            $("#tooltip").remove();
            previousPost = $(this).data('previous-post', -1);
        }
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
        }).appendTo("body").fadeIn(100);
    }




</script>

