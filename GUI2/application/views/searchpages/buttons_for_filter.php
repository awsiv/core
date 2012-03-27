<?php /* generate report and more filters button
 * 
 * set $more_filters to true to get more filters button
 * 
 *  */ ?>
<p class="alignrightbuttoncontainer">
    <span class="green_btn"><input class="green_btn" type="submit" value="Generate report"></input></span>
    <?php if (isset($morefilters) && $morefilters == true) { ?>
        <a  id="morefiltertoggle"class="green_btn" href="#"><span>More Filters</span></a>
        <script type="text/javascript">
            $('#morefiltertoggle').on('click',function(e){
                e.preventDefault();
                var textcontainer = $(this).find('span');
                if( textcontainer.text().toLowerCase()=='more filters'){
                    textcontainer.text('Fewer Filters') 
                }else {
                    textcontainer.text('More Filters') 
                }
                $('#morefilters').toggle();
            });
        </script>
    <?php } ?>
</p>