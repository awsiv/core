<div id="body">
    <div class="outerdiv">
        <div class="grid_4">
            <div id="navigation" class="innerdiv">
                <div id="astrolabe"></div>
            </div>
        </div>
        <div class="grid_8">
            <div class="innerdiv">
                <div style="padding: 15px">
                    <div id="astrolabeLocation" class="subTitle"></div>
                </div>
                
                <div style="padding: 15px">
                    <p class="subTitle"><?php echo $this->lang->line('header_promise_compliance'); ?></p>
                    <div class="graphcontainer">
                        <div id="hostsMeter" style="height:120px;"></div>
                    </div>
                </div>

                <div id="hostsComplianceContainer" style="padding: 15px">
                    <p class="subTitle"><?php echo $this->lang->line('header_host_status'); ?></p>
                    <div id="hostsCompliance"></div>
                </div>
                
                <div id="hostInfoContainer" style="padding: 15px">
                    <p class="subTitle"><?php echo $this->lang->line('host_info'); ?></p>
                    <div id="hostInfo"></div>
                </div>
            </div>
        </div>
        <div class="clear"></div>
    </div>
</div>
<div class="clear"></div>

<!-- TODO: move to carabiner later -->
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/astrolabe.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/astrolabeLocation.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/combobox.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/searchEntry.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/hostsCompliance.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/hostsMeter.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>/widgets/hostInfo.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>jScrollPane.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>jquery.mousewheel.js"></script>
<script type="text/javascript" src="<?php echo get_scriptdir()?>jquery.contextMenu.js"></script>

<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>astrolabe.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>astrolabeLocation.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>combobox.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>searchEntry.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>hostsCompliance.css" />
<link rel="stylesheet" type="text/css" href="<?php echo get_cssdir()?>jScrollPane.css" />

<script type="text/javascript">
    $(document).ready(function(){
        
        var genericOption = {
            baseUrl: '<?php echo site_url() ?>'
        };
        
        var hostFinderOption =   {
            baseUrl:'<?php echo site_url() ?>'
          };

        $.getJSON(genericOption.baseUrl + '/astrolabe/meters/', function(json) {
            $('#hostsMeter').hostsMeter({
                baseUrl: '<?php echo site_url() ?>',
                data: {
                    color: ['#779a62','#f0d76a','#cc4358'],
                    'label': json.labels,
                    'values': json.values
                }
            });
            $('#hostsMeter').hostsMeter("initialize");
        });
        
        $('#hostsCompliance').hostsCompliance();
        $('#hostInfo').hostInfo();
        $('#astrolabeLocation').astrolabeLocation();

        $('#astrolabe').astrolabe({
            hostSelected: function(event, args) {
                $('#hostsMeter').hostsMeter('updateHostKey', args.hostKey);

                $('#hostInfo').hostInfo('updateHostKey', args.hostKey);
                $('#hostInfoContainer').show();

                $('#hostsComplianceContainer').hide();

                $('#astrolabeLocation').astrolabeLocation('setHostName', args.hostName);
            },
            
            nodeSelected: function(event, args) {
                $('#hostInfoContainer').hide();

                $('#hostsCompliance').hostsCompliance('setContextPath',
                    args.includes, args.excludes);
                $('#hostsComplianceContainer').show();
                

                $('#astrolabeLocation').astrolabeLocation('setContextPath', args.path, args.count);
            }
        });
        
    });
</script>
