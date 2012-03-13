<div id="body">
    <div class="engineeringContainer">
        <div class="grid_4 alpha">
            <div class="engineeringNavigationContainer">
                <div id="astrolabe"></div>
            </div>
            <div class="clear"></div>
        </div>
        <div class="grid_8 omega" id="tabs-layout">
            <ul>
                <li><a href="#tab-1">Status</a></li>
                <li><a href="#tab-2">Reports</a></li>
            </ul>
            <div id="tab-1" class="engineeringStatusContainer">
                <div id="astrolabeLocation"></div>

                <div id="hostsComplianceTimeseriesContainer" class="hostsComplianceTimeseriesContainer">
                    <div id="hostsComplianceTimeseries" style="height: 200px"></div>
                </div>

                <div id="hostsComplianceContainer" class="hostsComplianceContainer">
                    <div id="hostsCompliance" class="grid_4 alpha"></div>
                    <div id="hostsConnectivity" class="grid_4 omega"></div>
                    <div class="clear"></div>
                </div>

                <div id="hostInfoContainer" class="hostInfoContainer">
                    <div id="hostInfo" class="grid_8"></div>
                    <div class="clear"></div>
                </div>
            </div>
            <div id="tab-2">
                <div id="reportInfoContainer" class="reportInfoContainer">
                    <div class="clear"></div>
                </div>
            </div>
        </div>
        <div class="clear"></div>
    </div>
</div>
<div class="clear"></div>

<!-- TODO: move to carabiner later -->
<script type="text/javascript" src="<?php echo get_scriptdir()?>/common.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/astrolabe.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/astrolabeLocation.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/combobox.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/searchEntry.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/hostsCompliance.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/hostsConnectivity.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/hostsComplianceTimeseries.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/hostInfo.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>jScrollPane.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>jquery.mousewheel.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>jquery.contextMenu.js"></script>


<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/notes.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/classfinder.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/contextfinder.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/reportUI.js"></script>



<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>view/engineering.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>astrolabe.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>astrolabeLocation.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>combobox.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>searchEntry.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>hostsCompliance.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>hostsConnectivity.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>hostInfo.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>jScrollPane.css" />

<script type="text/javascript">


    $(document).ready(function(){

        $("#tabs-layout").tabs();

        var genericOption = {
            baseUrl: '<?php echo site_url() ?>'
        };

        var hostFinderOption =   {
            baseUrl:'<?php echo site_url() ?>'
          };

        $('#hostsCompliance').hostsCompliance();
        $('#hostsConnectivity').hostsConnectivity();
        $('#hostInfo').hostInfo(genericOption);
        $('#astrolabeLocation').astrolabeLocation();
        $('#hostsComplianceTimeseries').hostsComplianceTimeseries();
        $('#reportInfoContainer').reportUI(genericOption);


        $('#astrolabe').astrolabe({
            hostSelected: function(event, args) {
                $('#hostInfo').hostInfo('updateHostKey', args.hostKey);
                $('#hostInfoContainer').show();

                $('#hostsComplianceTimeseries').hostsComplianceTimeseries('setHost',
                    args.hostKey);

                $('#hostsComplianceContainer').hide();
                $('#hostsConnectivityContainer').hide();

                $('#astrolabeLocation').astrolabeLocation('setHostName', args.hostName);
                $('#reportInfoContainer').reportUI('setHostContext',args.hostKey);
            },

            nodeSelected: function(event, args) {
                $('#hostInfoContainer').hide();

                $('#hostsComplianceTimeseries').hostsComplianceTimeseries('setContext', args.includes, []);

                $('#hostsCompliance').hostsCompliance('setContext', args.includes, []);
                $('#hostsComplianceContainer').show();

                $('#hostsConnectivity').hostsConnectivity('setContext', args.includes, []);
                $('#hostsConnectivityContainer').show();

                $('#astrolabeLocation').astrolabeLocation('setContextPath', args.path, args.count);

                $('#reportInfoContainer').reportUI('setContext',args.includes, args.excludes)
            }
        });

    });
</script>
