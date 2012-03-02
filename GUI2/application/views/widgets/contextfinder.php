<?php /*
<html>
<head>
<script type="text/javascript" src="http://dmitry.cf434lin.test.cfengine.com/scripts/jquery-1.6.1.min.js" charset="UTF-8"></script>
*/?>
 <style type="text/css">
 
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
<?php /*
</head>
<body>
    */?>
   
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
            <br clear="all">
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
            <br clear="all">
    </div> 
 </form>   
<a id="send" href="#">SEND </a>
</div>


<?php /*
<script type="text/javascript">

$(".add_line").live("click",  function(event) {
    console.log('asda');
  $(this).parent().clone().appendTo('.line_wrapper');
});

$(".swap").live("click",  function(event) {
   var tmp=$(this).parent().find('.include').val();
   $(this).parent().find('.include').val($(this).parent().find('.exclude').val());
   $(this).parent().find('.exclude').val(tmp)
});

$('#send').live("click",  function(event)  {
    alert($('#myform input').serialize());
});


//recursiveEncoded

</script>

</body>
</html> 
*/?>