
<div id="searchform" class="panel">
    <div class="panelhead"><?php echo $report_type ?> query</div><div class="panelcontent">
        <form method="post" action="<?php echo site_url('search') ?>">
            <p>Date:<input class="searchfield" type="text" name="days" ></p>
            <p>Months:<input class="searchfield" type="text" name="months" ></p>
            <p>Years:<input class="searchfield" type="text" name="years" ></p>
            <p>Host group: (.*+[])
                <a href="<?php echo site_url('widget/allclasses') ?>" id="bphghelp" class="help" 
                   title="<strong>SOFT CLASSES</strong> or <strong> HARD CLASSES</strong> 
                   with combination of regular expression as input, To open <strong>classfinder</strong> click me!">Help ?</a>
                <input class="searchfield" type="text" name="class_regex" size="80" id="hg"></p>
            <input type="hidden" name="host" value="All"">
                   <input type="hidden" name="report" value="<?php echo $report_type ?>">
            <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
            <p><input class="btn" type="submit" value="Generate report"> <span>       <a href="<?php echo site_url("/savedsearch/listSavedSearches/$report_type"); ?>"  class="slvbutton loadsavedsearch">Load saved searches</a>
                </span> </p>
        </form>
    </div>
</div>
<script type="text/javascript">
    $('.loadsavedsearch').ajaxyDialog({title:'Saved Searches'});

    $('#hg').smartTextBox({separator : "|"});
    $('#bphghelp').classfinder({
        defaultbehaviour:false,
        complete:function(event,data){
            //console.log(data.selectedclass);
            $('#hg') .smartTextBox('add', data.selectedclass);
        }
    });
    $('#bphghelp').qtip({
        style: {
            background:'#454545',
            opacity:0.9,
            name: 'dark',
            width:{
                min:0,
                max:150
            },
            border: {
                width: 0,
                radius: 4
            },
            tip:{
                corner:'topMiddle',
                size:{
                    x:10,
                    y:10
                }
            }
        },
        show:{
            effect:{
                type:'fade',
                lenght:200
            }
        },

        position: {
            corner: {
                target: 'bottomMiddle',
                tooltip: 'topMiddle'
            }
        }
    });
</script>