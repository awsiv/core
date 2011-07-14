<div id="body">
    <div class="outerdiv">       
        <div class="innerdiv">
            <div class="search">
                
                <?php require_once('compliance_by_promise.php'); ?>
                
            </div>
            
            
        </div>
        
        
        <div class="innerdiv">              
            <p class="title">Promise compliance summary by class</p>

            <?php
            $totalCount = $result['kept'] + $result['not_kept'] + $result['repaired'];
            ?>


                
                
                
                
                 <table border="1" cellpadding="1" cellspacing="1" class="bundlelist-table" style=" ">
                    <tbody>
                        <tr>
                            <th scope="col" colspan="3" style="text-align: center; ">
                                Hosts</th>
                            <th scope="col" rowspan="2" style="text-align: center; ">
                                Total hosts affected</th>
                            <th scope="col" rowspan="2" style="text-align: center; ">
                                Aggregated over interval</th>
                        </tr>
                        <tr>
                            <th scope="col" >
                                Compliant</th>                            
                            <th scope="col">
                                Repaired</th>
                            <th scope="col">
                                Non-compliant</th>
                        </tr>
                        <tr>
                            <td>
                                <span style="width:15px;display: inline-block;text-align: left;" class="greenhost">&nbsp;</span>
                                <?php 
                                
                                $link = sprintf('%s/search/index/state/c/hosts_only/true/host/All/report/Compliance summary/name/%s/class_regex/%s/',  site_url(),$handle,$classRegex); 
                                
                                ?>
                                
                                <a href="<?php echo $link; ?>"><span style="float: right; clear: both;">            
                                <?php echo $result['kept']; ?>
                                </span>
                                </a>    
                            </td>
                            <td>
                                <span style="width:15px;display: inline-block" class="yellowhost">&nbsp;</span><span style="float: right; clear: both;"><?php echo $result['repaired']; ?></span></td>
                            <td>
                                <span style="width:15px;display: inline-block" class="redhost">&nbsp;</span><span style="float: right; clear: both;"><?php echo $result['not_kept']; ?></span></td>
                            <td style="text-align: right; ">
                                <?php echo $result['host_count']; ?></td>
                            <td style="text-align: right; ">
                                Past <?php echo $timeFrame; ?></td>
                        </tr>
                        <tr>
                            <td colspan="5">
                                Classes : 
                                <?php if ($classRegex !='') { ?>
                                    <p><?php echo $classRegex; ?></p>
                                <?php } else {echo "any";} ?>                      
                                
                            </td>
                                
                        </tr>
                        <tr>
                            <td colspan="5">
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
