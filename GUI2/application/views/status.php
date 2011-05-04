
<div id="body" class="">
    <div class=" grid_9">
        <div class="outerdiv">
        <div id="overall" class="innerdiv">
            <p style="text-align: right;"><span id="alltime"></span><span id="thisweek"></span><span id="today"></span></p>


            <div id="pie-charts">

                <div id="business-value-pie-chart-container" class="grid_4">
                    <p class="title">Business Value</p>
                    <div id="business-value-pie-chart" style="height: 150px;width:300px;">
                    </div>
                </div>
                <div id="compliance-value-pie-chart-container" class="grid_4 push_1">
                    <p class="title">Compliance Now</p>
                    <div id="compliance-now-pie-chart" style="height: 150px; width:300px;">
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
            <div id="compliancemeter" class="">
                <div id="compliance_summary_graph" style="height: 200px; width:98%;"></div>
                <div class="clear"></div>
            </div>
        </div>
        </div>
     </div>
    <div class="grid_3">
        <div class="outerdiv ">
            <div id="announcement" class="innerdiv">
                <p class="title">Announcement</p>
            </div>
            <div id="goals" class="innerdiv">
                <p class="title">Services/goals</p>
                <ul>
                    <?php
                        foreach ($goals as $goal) {
                            $words = explode("_", $goal->name);
                            echo "<li><span class=\"goal\">$words[0] $words[1]</span> - <span>$goal->desc</span><span class=\"check\"></span></li>";
                        }
                    ?>
                    </ul>
                </div>
        </div>
    </div>
</div>
    <script type="text/javascript">
        // for summary compliance graph

        var graphEnvironment = '';
        var json = {
            color: ['#6c9053','#cfb93f','#D43030','#5C5858'],
            'label': <?php echo $compliance_summary['graphSeries']['labels']; ?>,
            'values':  <?php echo $compliance_summary['graphSeries']['values']; ?>
        };
        var countTrack =  <?php echo $compliance_summary['graphSeries']['count']; ?>;
        var startTrack =  <?php echo $compliance_summary['graphSeries']['start']; ?>;

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

        var barChartOptions = {
            //id of the visualization container
            injectInto: 'compliance_summary_graph',
            //whether to add animations
            animate: true,
            type: 'stacked',//useGradients? 'stacked:gradient' : 'stacked',
            //labels style
            Label: {
                type: labelType, //Native or HTML
                size: 13,
                family: 'Arial',
                color: 'black'
            },
            showLabels:false,
            barsOffset:2,
            useGradient: useGradients? 'stacked:gradient' : 'stacked',
            orientation: 'vertical',
            //bars separation
            //visualization offset
            Margin: {
                top:5,
                left: 10,
                right: 5,
                bottom:5
            },
            //labels offset position

            labelOffset: 5,
            //bars style
            //whether to show the aggregation of the values
            showAggregates:false,
            //whether to show the labels for the bars
            showLabels:false,
            hoveredColor:false,
            Tips: {
                enable: true,
                onShow: function(tip, elem) {
                    var label = elem.label;
                    var count = countTrack[label];
                    var string = label + '<br />' + 'Value :: ' + elem.value + '<br />' + 'Count :: ' + count;
                    tip.innerHTML =  string;
                }
            },
            Events: {
                enable: true,
                onClick: function(node, eventInfo, e) {
                    if ( startTrack[node.label]) {
                        //var option = {'url':'/widget/summaryCompliance/'+startTrack[node.label],'title':'Overview'};
                        var element = $('<a href="/widget/summaryCompliance/'+startTrack[node.label]+ '/' + graphEnvironment +'" title="overview" />');
                        element.ajaxyDialog().ajaxyDialog("open");
                    }
                }
            }
        };

        $barChart = new $jit.BarChart(barChartOptions);
        $barChart.loadJSON(json);


        $('.updateComplianceGraph').click(function(){
            var url = $(this).attr('href');
            var env = $(this).attr('title');
            $(this).parent().siblings(".selected").removeClass("selected").find('.tippointer').remove();
            $(this).parent().addClass('selected');
            $('<div class="tippointer">').css({'margin-top':5,'left':$(this).parent().width()/2 -6}).appendTo($(this).parent());

            $.getJSON(url, function(data) {

                var json = data.graphdata[0];
                // update count and start track as well
                countTrack =  data.countData;
                startTrack =  data.startData;
                //update the graph environment variable
                graphEnvironment = env;


                $barChart.updateJSON(json);
            });
            return false;
        })


        // business value pie chart
        var piejson = {
            'color': ["#90A316","#BFAA54","#C33D54"],
            'label': 'label A',
            'values': [
                {
                    'label': 'kept',
                    'values': '<?php echo $businessValuePie['kept']; ?>'
                },
                {
                    'label': 'repaired',
                    'values': '<?php echo $businessValuePie['repaired']; ?>'
                },
                {
                    'label': 'not kept',
                    'values': '<?php echo $businessValuePie['notkept']; ?>'
                }]
        };


        // host count pie chart
        var hostpiejson = {
            'color': ["#90A316","#BFAA54","#C33D54"],
            'values': [
                {
                    'label': 'green',
                    'values': '<?php echo $greenhost; ?>'
                },
                {
                    'label': 'yellow',
                    'values': '<?php echo $yellowhost; ?>'
                },
                {
                    'label': 'red',
                    'values': '<?php echo $redhost; ?>'
                }]
        };

        //init PieChart
        /*
    var pie = new $jit.PieChart({
        injectInto: 'business-value-pie-chart',
        animate: true,
        offset: 10,
        sliceOffset: 2,
        labelOffset: 0,
        type:'stacked:gradient',
        updateHeights:false,
        showLabels:false,
        Label: {
            size: 18,
            family: 'Arial',
            color: 'black'
        },
        Tips: {
            'enable': true,
            'onShow': function(tip, elem) {
                tip.innerHTML = "<span style='color:black;'><b>" + elem.label + "</b>: " + elem.value + "</span>";
            }
        }
    });


    var hostpie = new $jit.PieChart({
        injectInto: 'compliance-now-pie-chart',
        animate: true,
        offset: 10,
        sliceOffset: 2,
        labelOffset: 0,
        type:'stacked:gradient',
        updateHeights:false,
        showLabels:false,
        Label: {
            size: 18,
            family: 'Arial',
            color: 'black'
        },
        Tips: {
            'enable': true,
            'onShow': function(tip, elem) {
                tip.innerHTML = "<span style='color:black;'><b>" + elem.label + "</b>: " + elem.value + "</span>";
            }
        }
    });

    //load JSON data.
    pie.loadJSON(piejson);
    hostpie.loadJSON(hostpiejson);
         */



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


        var complianceNowData =  [
            { label: "green",  data: <?php echo $greenhost; ?>,color:"#90A316"},
            { label: "yellow",  data: <?php echo $yellowhost; ?>,color:"#C33D54"},
            { label: "red",  data: <?php echo $redhost; ?>,color:"#BFAA54"}
    ];
    $.plot($("#compliance-now-pie-chart"), complianceNowData,
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

    
    $("#business-value-pie-chart").bind("plothover", pieHover);
    $("#compliance-now-pie-chart").bind("plothover", pieHover);
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

