
<div id="bodystatus">
    <div class="left"></div>
    <div class="middle minwidth80">
        <div id="overall" class="innerdiv">
            <p style="text-align: right;"><span id="alltime"></span><span id="thisweek"></span><span id="today"></span></p>


            <div id="pie-charts">

                <div id="business-value-pie-chart-container" class="grid_6">
                    <p class="title">Business Value</p>
                    <div id="business-value-pie-chart" style="height: 150px;width:200px;">


                    </div>




                </div>
                <div id="business-value-pie-chart-container" class="grid_6">
                    <p class="title">Compliance Now</p>
                    <div id="compliance-now-pie-chart" style="height: 150px; width:200px;">


                    </div>




                </div>





            </div>




        </div>
        <div id="compliance" class="innerdiv">
            <p class="title">Compliance Summary</p>
            <div id="environment-list">
                <ul>
                    <li><!-- chrome fix --></li>
                    <li><span class="front"></span><a class ="updateComplianceGraph" href="/welcome/getJsonComplianceSummary/">Default</a><span class="rear"></span></li>
                    <?php foreach ($envList as $key => $env) { ?>
                        <li><span class="front"></span><a class ="updateComplianceGraph" href="/welcome/getJsonComplianceSummary/<?php echo $env; ?>"><?php echo $env; ?></a><span class="rear"></span></li>
                    <?php } ?>
                </ul>
            </div>
            <div id="compliancemeter" class="">
                <div id="compliance_summary_graph" style="height: 200px; width:98%;"></div>
                <div class="clear"></div>
            </div>
        </div>

    </div>
    <div class="right"></div>

    <div class="left"></div>
    <div class="middle minwidth20">
        <div id="announcement" class="innerdiv">
            <p class="title">Announcement</p>
        </div>
        <div id="goals" class="innerdiv">
            <p class="title">Services/goals</p>
            <ul>
            <?php
            foreach($goals as $goal)
             {
               $words = explode("_",$goal->name);
               echo "<li><span class=\"goal\">$words[0] $words[1]</span> - <span>$goal->desc</span><span class=\"check\"></span></li>";
             }
            ?>
            </ul>
        </div>
    </div>
    <div class="right"></div>
    <div class="clearboth"></div>
</div>

<div class="clear"></div>

<div id="controltest">

</div>
<script type="text/javascript">
    // for summary compliance graph
    var json = {
        color: ['#A3DF00','#EEEE00','#D43030','#5C5858'],
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
        type: useGradients? 'stacked:gradient' : 'stacked',
        //labels style
        Label: {
            type: labelType, //Native or HTML
            size: 13,
            family: 'Arial',
            color: 'black'
        },
        showLabels:false,
        barsOffset:0,
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
                    var element = $('<a href="/widget/summaryCompliance/'+startTrack[node.label]+ '" title="overview" />');
                    element.ajaxyDialog().ajaxyDialog("open");
                }
            }
        }
    };

    $barChart = new $jit.BarChart(barChartOptions);
    $barChart.loadJSON(json);
    $('.updateComplianceGraph').click(function(){
        var url = $(this).attr('href');
        $(this).parent().siblings(".selected").removeClass("selected").find('.tippointer').remove();
        $(this).parent().addClass('selected');
        $('<div class="tippointer">').css({'margin-top':5,'left':$(this).parent().width()/2 -6}).appendTo($(this).parent());

        $.getJSON(url, function(data) {

            var json = data.graphdata[0];
            // update count and start track as well
            countTrack =  data.countData;
            startTrack =  data.startData;
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
                'values': Math.abs(<?php echo $businessValuePie['kept']; ?>)
            },
            {
                'label': 'repaired',
                'values': Math.abs(<?php echo $businessValuePie['repaired']; ?>)
            },
            {
                'label': 'not kept',
                'values': Math.abs(<?php echo $businessValuePie['notkept']; ?>)
            }]
    };

    
    //init PieChart
    pie = new $jit.PieChart({
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

    //load JSON data.
    pie.loadJSON(piejson);





</script>

