<div class="pagepanel">
    <div class="panelhead">List of Hosts</div>
    <div class="panelcontent">
         <form method="post" action="<?php echo site_url('search/index')?>">
         <label class="grid_2">Filter by Class pattern</label><input class="grid_8" type="text" name="name" id="classlist" >
         <input type="hidden" name="manyhosts" value="true">
         <input type="hidden" name="report" value="Class profile">
         <input type="hidden" name="hosts_only" value="true">
         <input class="btn" type="submit" value="Filter" class="grid_1">
         </form>
        <div class="clear"></div>
        <div id="hostlist" class="panelcontent tables">
        <?php
        if (count($tabledata)>0)
           {
            echo "<table><tr>";
            for($i=0;$i<count($tabledata);$i++)
            {
                if($i%6==0 && $i!=0)
                {
                    echo '</tr><tr>';
                }
                echo '<td>'.$tabledata[$i].'</td>';
            }
            echo "</tr></table>";
           }
        ?>
      </div>
    </div>
</div>
<script type="text/javascript">
    $(document).ready(function(){
    var options={
        success:function(data){
               $('#hostlist').html($(data).find('.tables'));
        }
    }
    $('#tabpane').find('form').ajaxForm(options);
    //var queryString = $('#myFormId').formSerialize();
    function split( val ) {

			return val.split( /\|/ );
		}
		function extractLast( term ) {
			return split( term ).pop();
		}

		$( "#classlist" ).autocomplete({
                        minLength: 2,
                        source: function( request, response ) {
					// delegate back to autocomplete, but extract the last term
					response( $.ui.autocomplete.filter(
						<?php echo $classes?>, extractLast( request.term ) ) );
				},
				focus: function() {
					// prevent value inserted on focus
					return false;
				},
				select: function( event, ui ) {
					var terms = split( this.value );
					// remove the current input
					terms.pop();
					// add the selected item
					terms.push( ui.item.value );
					// add placeholder to get the comma-and-space at the end
					terms.push( "" );
					this.value = terms.join( "|" );
					return false;
				}
		});
    })
</script>