 <style type="text/css">
 /* this styles only for testing*/
    .line {
        margin-bottom: 20px;
    }
    
    .line:nth-child(2n) {
    background:#e6eee3;
}

.line:hover {
    background-color: #f7f7f7
}
    .item {
        width: 120px;
        float:left;
    }
    .item input {width: 80px;}
    
    .swap {
        width: 70px;
        margin: 0 20px;
        float: left;
    }
    .add_line {
        margin-left: 20px;
    }
</style>   
   
<div class="line_wrapper">
  <form id="myform"> 
    <div class="line" >
            <div class="item">
                <input type="text" name="include[]" class="include" value=""> 
                <a class="class_selector" id="b" href="<?php echo site_url('widget/allclasses') ?>">[+]</a>
            </div>
        
            <div class="swap"><a href="#">[<= =>]</a></div>
       
             <div class="item">
                    <input type="text"  name="exclude[]" class="exclude" value="">
                    <a class="class_selector" id="c" href="<?php echo site_url('widget/allclasses') ?>">[+]</a>   
             </div>
         
            <a href="#" class="add_line">+ Add line</a>
            <div class="clearboth"></div>
    </div> 
        <div class="line" >
            <div class="item">
                <input type="text" name="include[]" class="include" value=""> 
                <a class="class_selector" id="b" href="<?php echo site_url('widget/allclasses') ?>">[+]</a>
            </div>
        
            <div class="swap"><a href="#">[<= =>]</a></div>
       
             <div class="item">
                    <input type="text"  name="exclude[]" class="exclude" value="">
                    <a class="class_selector" id="c" href="<?php echo site_url('widget/allclasses') ?>">[+]</a>   
             </div>
         
            <a href="#" class="add_line">+ Add line</a>
            <div class="clearboth"></div>
    </div> 
 </form>   
<a id="send" href="#">SEND </a>
</div>