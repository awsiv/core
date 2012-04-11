<div id="body" class="outerdiv">
    <div class="engineeringContainer">
        <div class="grid_4 alpha">
            <div class="engineeringNavigationContainer">
                <div id="astrolabe"></div>
            </div>
            <div class="clear"></div>
        </div>
        <div id="darktabs">
        <div class="grid_8 omega" id="tabs-layout">
            <ul>
                <li class="first"><a href="#tab-1">Status</a></li>
                <li><a href="#tab-2">Reports</a></li>
            </ul>
            <div id="tab-1" class="engineeringStatusContainer">
                <div class="astrolabeLocation"></div>

                <div id="hostsComplianceTimeseriesContainer" class="hostsComplianceTimeseriesContainer">
                    <span id="hostsComplianceTimeseriesLoader" class="loading"></span>
                    <div id="hostsComplianceTimeseries"></div>
                </div>

                <div id="hostsInfoContainer" class="hostsInfoContainer">
                    <div id="hostsComplianceContainer" class="hostsComplianceContainer">
                      <div id="hostsCompliance" class="hostsCompliance"></div>
                    </div>
                    <div class="hostsConnectivityContainer">
                        <div id="hostsConnectivity" class="hostsConnectivity"></div>
                    </div>
                     <div class="clear"></div>
                </div>
              

                <div id="hostInfoContainer" class="hostsInfoContainer">
                    <div id="hostInfo" class="grid_8"></div>
                    <div class="clear"></div>
                </div>
                <div class="clear"></div>
            </div>
            <div id="tab-2">
                    <div class="astrolabeLocation"></div>
                <div id="reportInfoContainer" class="reportInfoContainer">
                    <div class="clear"></div>
                </div>
            </div>
        </div>
    </div>
        <div class="clear"></div>
    </div>
    <div class="clear"></div>
</div>
<div class="clear"></div>
<script type="text/javascript">


    $(document).ready(function(){

        var $tabs=$("#tabs-layout");
            $tabs.tabs({});
                  
        var genericOption = {
            baseUrl: '<?php echo site_url() ?>'
        };

        $('#hostsCompliance').hostsCompliance(genericOption);
        $('#hostsConnectivity').hostsConnectivity(genericOption);
        $('#hostInfo').hostInfo(genericOption);
        $('.astrolabeLocation').astrolabeLocation(genericOption);
        $('#hostsComplianceTimeseries').hostsComplianceTimeseries();
        $('#reportInfoContainer').reportUI(genericOption);

        $('#astrolabe').astrolabe({
            baseUrl: '<?php echo site_url() ?>',

            hostSelected: function(event, args) {
                 $tabs.data({context:'host',args:args});
                $('#hostInfoContainer').show();
                $('#hostsInfoContainer').hide();

                $('#hostInfo').hostInfo('updateHostKey', args.hostKey);

                $('#hostsComplianceTimeseries').hostsComplianceTimeseries('setHost',
                    args.hostKey);

                $('.astrolabeLocation').astrolabeLocation('setHostName', args.hostName);
                $('#reportInfoContainer').reportUI('setHostContext',args.hostKey);
            },

            nodeSelected: function(event, args) {
                $tabs.data({context:'node',args:args});
                $('#hostInfoContainer').hide();
                $('#hostsInfoContainer').show();
                
                $('#hostsComplianceTimeseries').hostsComplianceTimeseries('setContext', args.includes, []);

                $('#hostsCompliance').hostsCompliance('setContext', args.includes, []);

                $('#hostsConnectivity').hostsConnectivity('setContext', args.includes, []);

                $('.astrolabeLocation').astrolabeLocation('setContextPath', args.path, args.count);

                $('#reportInfoContainer').reportUI('setContext',args.includes, args.excludes)
            }
        });

        //calculate height for panesl

        var tab1_outh = $('#tab-1').outerHeight(true);
        var tab1_h = $('#tab-1').height();

        var tab2_outh = $('#tab-2').outerHeight(true);
        var tab2_h = $('#tab-2').height();

        var astro_outh = $('#astrolabe .containerWrapper').outerHeight(true);
        var astro_h = $('#astrolabe .containerWrapper').height();

        var max = Math.max(tab1_outh, tab2_outh, astro_outh);

        $('#tab-1').height( (tab1_h + (max-tab1_outh)) );
        $('#tab-2').height( (tab2_h + (max-tab2_outh)) );

        $('#astrolabe .containerWrapper').height( (astro_h + (max-astro_outh)) );
    });
</script>
