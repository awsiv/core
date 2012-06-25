<div id="body" class="outerdiv">
    <div class="engineeringContainer">
        <div class="grid_4 alpha">
            <div class="engineeringNavigationContainer">
                <div id="astrolabe"></div>
                     <div class="clear"></div>
            </div>
               
            <div class="clear"></div>
        </div>
        <div id="darktabs">
            <div class="grid_8 omega" id="tabs-layout">
            <ul>
                <li class="first"><a href="#tab-1">Status</a></li>
                <li><a href="#tab-2">Reports</a></li>
                <li><a href="#tab-3">Events</a></li>
            </ul>
            <div id="tab-1" class="engineeringStatusContainer">
                <div class="astrolabeLocation"></div>
                <div class="clear"></div>
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
            <div id="tab-2" style="padding-right: 0">
                    <div class="astrolabeLocation"></div>
                <div id="reportInfoContainer" class="reportInfoContainer">
                    <div class="clear"></div>
                </div>
            </div>
             <div id="tab-3">
                    <div class="astrolabeLocation"></div>
                    <div id="event_viewer">
                        
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
        $('#hostsInfoContainer').show();

        $('#hostInfo').hostInfo(genericOption);
        $('#hostInfoContainer').hide();

        $('.astrolabeLocation').astrolabeLocation(genericOption);
        $('#hostsComplianceTimeseries').hostsComplianceTimeseries({baseUrl: '<?php echo site_url() ?>'});
        $('#reportInfoContainer').reportUI(genericOption);
        $('#event_viewer').eventTrackerUI(genericOption);

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
                $('#event_viewer').eventTrackerUI('setHostContext',args.hostKey);
            },

            nodeSelected: function(event, args) {
                $tabs.data({context:'node',args:args});
                $('#hostInfoContainer').hide();
                $('#hostsInfoContainer').show();

                $('#hostsComplianceTimeseries').hostsComplianceTimeseries('setContext',
                    args.includes, [], args.count);

                $('#hostsCompliance').hostsCompliance('setContext', args.includes, []);

                $('#hostsConnectivity').hostsConnectivity('setContext', args.includes, []);

                $('.astrolabeLocation').astrolabeLocation('setContextPath', args.path, args.count);

                $('#reportInfoContainer').reportUI('setContext',args.includes, []);
                $('#event_viewer').eventTrackerUI('setContext',args.includes,[]);
            }
        });
    });
</script>
