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
        <?php echo anchor('test/cfclasses','help',array('id'=>'getclass')) ?>
        <?php echo anchor('#','hostfinder',array('id'=>'findhost')) ?>
        <?php echo anchor('widget/allclasses','classfinder',array('id'=>'findclass')) ?>
        <?php echo anchor('widget/allpolicies','policyfinder',array('id'=>'findpolicy')) ?>
        <?php echo anchor('widget/allreports','reports',array('id'=>'findreport')) ?>
        <div class="clear"></div>
        <!--<div id="filters"><span>Selected classes:</span><ul></ul></div>-->
        <div id="hostlist" class="panelcontent tables">
        <?php
           echo $hoststable;
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

		$( "#classlist" ).bind( "keydown", function( event ) {
				if ( event.keyCode === $.ui.keyCode.TAB &&
						$( this ).data( "autocomplete" ).menu.active ) {
					event.preventDefault();
				}
			}).autocomplete({
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
            $('#findhost').hostfinder();

            $('#findclass').classfinder();

            $('#findpolicy').policyfinder();

            $('#findreport').reportfinder();

          // var $filter=$('#filters').find('ul');

          /* $('#getclass').classtags({
                   url:"/test/cfclasses",
                   complete:function(event,data){
                       //console.log(data.selectedclass);
                       var li = $("<li>");
                       li.text(data.selectedclass).data('filter',data.selectedclass).appendTo($filter);
                       $("<a>").text("X").appendTo(li)
                       //$filter.append('<li>'+data.selectedclass+'<a href="#" title="removefilter">X</a></li>');
                       var filters=new Array();
                               $filter.find('li').each(function(index) {
                                    filters.push($(this).data('filter'));
                               });
                      $('#hostlist').load('/welcome/ajaxlisthost/',{'filter':filters},function(){});
                   }
               });

           

           $filter.delegate('a','click',function(event){
                              event.preventDefault();
                              $(this).parent().remove();
                              var filters=new Array();
                              /*if($filter.find('li:last').data('filter')){
                              $('#hostlist').load('/welcome/listhost/'+$filter.find('li:last').data('filter'));
                              }
                              else{
                                $('#hostlist').load('/welcome/listhost/NULL');
                              }
                               $filter.find('li').each(function(index) {
                                    filters.push($(this).data('filter'));
                               });
                               $('#hostlist').load('/welcome/ajaxlisthost/',{'filter':filters},function(){});
                               

                       });*/
    })
</script>