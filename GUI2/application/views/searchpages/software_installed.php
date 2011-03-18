<div id="searchform" class="panel">
  <div class="panelhead"><?php echo $report_type?> query</div>
    <div class="panelcontent">
          <form method="post" action="<?php echo site_url('search')?>">
          <p>On Host:<?php echo form_dropdown('host', $options,'All','id="host"');?></p>
          <p>Name: <input class="searchfield" type="text" name="name" id="packagename"></p>
          <p>Version:<input class="searchfield" type="text" name="version" size="80" id="version"></p>
          <p>Architecture:<input class="searchfield" type="text" name="arch" size="80" id="arc"></p>
          <p>Host group: (.*+[])<input class="searchfield" type="text" name="class_regex" size="80"></p>
          <!--<input type="hidden" name="manyhosts" value="true">-->
          <input type="hidden" name="report" value="<?php echo $report_type?>">
          <p>Return hostnames only: <input type="checkbox" name="hosts_only" value="true"></p>
          <p><input class="btn" type="submit" value="Generate report"></p>
          </form>
     </div>
  </div>
<script type="text/javascript" src='<?php echo get_scriptdir()?>widgets/combo.js'></script>
<script type="text/javascript">
    
$(function() {
     
    function split( val ) {
       
			return val.split( /\|/ );
		}
		function extractLast( term ) {
			return split( term ).pop();
		}

		$( "#packagename" )
                .bind( "keydown", function( event ) {
				if ( event.keyCode === $.ui.keyCode.TAB &&
						$( this ).data( "autocomplete" ).menu.active ) {
					event.preventDefault();
				}
			})
                .autocomplete({
                        minLength: 2,
                        source: function( request, response ) {
					// delegate back to autocomplete, but extract the last term
					response( $.ui.autocomplete.filter(
						<?php echo $package_name_list?>, extractLast( request.term ) ) );
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

           $( "#arc" ).autocomplete({                 
                 source:<?php echo $architecture?>
          });

           $( "#version" ).autocomplete({
                 minLength: 2,
                 source:<?php echo $version?>
          });
          //for combo box
          $( "#host" ).combobox({selected:function()
                           {
                               if($(this).val()!="All")
                                   {
                                     $(this).parent().parent().find('input.searchfield').attr('disabled',true);
                                   }
                               else
                                   {
                                       $(this).parent().parent().find('input.searchfield').removeAttr('disabled');
                                   }
                               
                           }});
        });

</script>

