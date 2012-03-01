<div id="body">
    <div class="engineeringContainer">
        <div class="grid_4 alpha">
            <div class="engineeringNavigationContainer">
                <div id="astrolabe"></div>
            </div>
        </div>
        <div class="grid_8 omega">
            <div class="engineeringContentContainer">
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

        var genericOption = {
            baseUrl: '<?php echo site_url() ?>'
        };

        var hostFinderOption =   {
            baseUrl:'<?php echo site_url() ?>'
          };

        $('#hostsCompliance').hostsCompliance();
        $('#hostsConnectivity').hostsConnectivity();
        $('#hostInfo').hostInfo();
        $('#astrolabeLocation').astrolabeLocation();
        $('#hostsComplianceTimeseries').hostsComplianceTimeseries();

        $('#astrolabe').astrolabe({
            hostSelected: function(event, args) {
                $('#hostInfo').hostInfo('updateHostKey', args.hostKey);
                $('#hostInfoContainer').show();

                $('#hostsComplianceContainer').hide();
                $('#hostsConnectivityContainer').hide();

                $('#astrolabeLocation').astrolabeLocation('setHostName', args.hostName);
            },

            nodeSelected: function(event, args) {
                $('#hostInfoContainer').hide();

                $('#hostsComplianceTimeseries').hostsComplianceTimeseries('setContext',
                    args.includes, args.excludes);

                $('#hostsCompliance').hostsCompliance('setContextPath',
                    args.includes, args.excludes);
                $('#hostsComplianceContainer').show();

                $('#hostsConnectivity').hostsConnectivity('setContextPath',
                    args.includes, args.excludes);
                $('#hostsConnectivityContainer').show();

                $('#astrolabeLocation').astrolabeLocation('setContextPath', args.path, args.count);
            }
        });

    });
</script>
