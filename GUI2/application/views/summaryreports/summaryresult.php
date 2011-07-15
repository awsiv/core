<div id="body">
    <div class="outerdiv">       
        <div class="innerdiv">
            <div id="collapse">
            <h3><a href="#">New search</a></h3>
            <div class="search">
                
                <?php require_once('compliance_by_promise.php'); ?>
                
            </div>
            </div>
            
        </div>
        
        
        <div class="innerdiv">              
            <p class="title">Promise compliance summary by given class</p>

            <?php
            $totalCount = $result['kept'] + $result['not_kept'] + $result['repaired'];
            ?>


                
                
                
                
                 <table border="1" cellpadding="1" cellspacing="1" class="bundlelist-table" style=" ">
                    <tbody>
                        <tr>
                            <th scope="col" colspan="2" rowspan="1" style="text-align: center; ">
                                Compliant hosts</th>
                            <th scope="col" rowspan="1" style="text-align: center">Non-compliant hosts</th>
                             <th scope="col" rowspan="1" style="text-align: center; ">
                                Total hosts verified</th>  
                            <th scope="col" rowspan="1" style="text-align: center">Unreachable hosts</th>
                           
                            <th scope="col" colspan="1" rowspan="1" style="text-align: center; ">
                                This was last verified between</th>
                        </tr>
                      
                        <tr>
                            <td>
                                <span style="width:15px;display: inline-block;text-align: left;" class="greenhost">&nbsp;</span>
                              <span style="float: right; clear: both;">            
                                <?php echo $result['kept']; ?>
                                </span>
                               
                            </td>
                            <td>
                                <span style="width:15px;display: inline-block" class="yellowhost">&nbsp;</span><span style="float: right; clear: both;"><?php echo $result['repaired']; ?></span></td>
                            <td>
                                <span style="width:15px;display: inline-block" class="redhost">&nbsp;</span><span style="float: right; clear: both;"><?php echo $result['not_kept']; ?></span></td>
                             <td style="text-align: right; ">
                                <?php echo $result['host_count']; ?></td>
                            <td>
                                <span style="width:15px;display: inline-block" class="bluehost">&nbsp;</span><span style="float: right; clear: both;"> <?php echo $result['code_blue']; ?></span></td>
                           
                            <td style="text-align: center; ">
                                 <?php echo getDateStatus($result['start'],true); ?> -  <?php echo getDateStatus($result['end'],true); ?></td>
                        </tr>
                        <tr>
                            <td colspan="6">
                                Classes : 
                                <?php if ($classRegex !='') { ?>
                                    <p><?php echo $classRegex; ?></p>
                                <?php } else {echo "any";} ?>                      
                                
                            </td>
                                
                        </tr>
                        <tr>
                            <td colspan="6">
                                Promise handles :
                               
                                <?php if ($handle !='') { ?>
                                    <p><?php echo $handle; ?></p>
                                <?php } else {echo "All";} ?>                     
                                
                            </td>
                                
                        </tr>
                    </tbody>
                </table>




            </div>



        </div>
</div>
<script type="text/javascript">
    $(function() {
       
        // accordian search 
        $("#collapse").addClass("ui-accordion ui-accordion-icons ui-widget ui-helper-reset")
        .find("h3")
        .addClass("ui-accordion-header ui-helper-reset ui-state-default ui-corner-top ui-corner-bottom")
        .hover(function() { $(this).toggleClass("ui-state-hover"); })
        .prepend('<span class="ui-icon ui-icon-triangle-1-e"></span>')
        .click(function() {
            $(this)
            .toggleClass("ui-accordion-header-active ui-state-active ui-state-default ui-corner-bottom")
            .find("> .ui-icon").toggleClass("ui-icon-triangle-1-e ui-icon-triangle-1-s").end()
            .next().slideToggle().toggleClass("ui-accordion-content-active");
            var $link = $('.ui-tabs-nav li:eq(1) a',$(this).next());       
            $link.trigger('click');
            return false;
        })
        .next()
        .addClass("ui-accordion-content  ui-helper-reset ui-widget-content ui-corner-bottom")
        .hide();

        $("#collapse")
        .find("h3 a")
        .focus(function() { $(this).parent().toggleClass("ui-state-hover"); })
        .focusout(function() { $(this).parent().toggleClass("ui-state-hover"); })
        
        
        
    });
</script>