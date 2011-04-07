<?php
#
# This file is (C) Cfengine AS. All rights reserved
#
?>       
<div class="grid_4">

    <div class="panel">
        <div class="panelhead"> View Reports </div>
        <div id="reportcontrol" class="panelcontent">

        </div>
    </div>
    <div class="panel">
        <div class="panelhead"> Hosts</div>
        <ul class="panelcontent">
            <li>Hosts Known: <?php echo $all ?></li>
            <li>Ranked worst: <a href="weakest_host">Show top 50</a></li>
        </ul>
    </div>
</div>
<div class="grid_8">
    <div id="repdialog" class="hidden"></div>
    <div class="panel">
        <div class="panelhead">Average meters summarizing all hosts</div>
        <div class="panelcontent">
            <ul>
                <li><?php include_once('graph/summaryCompliance.php'); ?></li>
                <li><img class="icontext" src="<?php echo get_imagedir(); ?>/host.png"> <a href="<?php echo site_url('/welcome/host') ?>">Select a particular host</a></li>
            </ul>

        </div>
    </div>

    <div class="panel">
        <div class="panelhead">Compliance Summary</div>
        <div class="panelcontent">
            <div style="border-style: dotted;">
                <ul style="display: inline;">
                    <li><a class ="updateComplianceGraph" href="/welcome/getJsonComplianceSummary/">Default</a></li>

                    <?php foreach ($envList as $key => $env) {
 ?>
                        <li><a class ="updateComplianceGraph" href="/welcome/getJsonComplianceSummary/<?php echo $env; ?>"><?php echo $env; ?></a></li>
<?php } ?>
                </ul>
            </div>
            <div id="compliance_summary_graph" style="height: 200px; width:95%;"></div>
            <div class="clear"></div>
            
        </div>
    </div>




    <div class="panel">
        <div class="panelhead">Summary of Content-Driven Policies</div>
        <div class="panelcontent">
            <form method="post" action="<?php echo site_url('cdp/index') ?>">
                <p><?php echo $allSppReps; ?><p>
                    <input class="btn" type="submit" value="Generate">
            </form>
        </div>
    </div>

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
                  var option = {'url':'/widget/summaryCompliance/'+startTrack[node.label],'title':'Overview'};
                  $('body').popup(option);
                }
            }
        }
    };

    $barChart = new $jit.BarChart(barChartOptions);
    $barChart.loadJSON(json);
    $('.updateComplianceGraph').click(function(){
    var url = $(this).attr('href');
        $.getJSON(url, function(data) {

            var json = data.graphdata[0];
            // update count and start track as well
            countTrack =  data.countData;
            startTrack =  data.startData;
            $barChart.updateJSON(json);
        });
        return false;
    })



    reportcontrol(<?php echo $jsondata ?>,"overall");
    reportcontrol2(<?php echo $jsondata2 ?>,"overall");

    
</script>

