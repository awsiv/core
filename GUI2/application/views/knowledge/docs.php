<div id="body" class="grid_12 outerdiv">
    <div class="innerdiv">
        <?php foreach ((array) $docs as $category) { ?>
            <p><h3><?php echo $category['category']; ?></h3></p>
            <p class ="<?php echo $category['category']; ?>">
                <?php foreach ((array) $category['files'] as $files) { ?>


                    <span style="padding:5px;"><a href="<?php echo base_url() . 'docs/' . $files['filename']; ?>"><?php echo $files['title']; ?></a></span>   


                <?php } ?>
            </p>
        <?php } ?> 
    </div>
</div>
<div id="preview" style="display:none;border: 2px;left:25%;top:25%;position: absolute;background: #FFFFFF;z-index: 10;margin: 0 auto;"></div>
<div id="background" style="display:none;border: 2px;position: absolute;background-color: #000000;height: 100%; width: 100%;top: 0;left: 0;z-index:2"></div>
<script type="text/javascript">
    $(document).ready(function() {
        
        jQuery.fn.center = function () {
            this.css("position","absolute");
            this.css("top", (($(window).height() - this.outerHeight()) / 2) + $(window).scrollTop() + "px");
            this.css("left", (($(window).width() - this.outerWidth()) / 2) + $(window).scrollLeft() + "px");
            return this;
        }
        $(".Illustrations a").click(function(e){
            
            e.preventDefault();
            $("#background").css({"opacity" : "0.7"})
            .fadeIn("slow");
 
            $("#preview").html("<img src='"+$(this).attr('href')+"' />")            
            .fadeIn("slow");
            
          
            return false;
        });
        
        $(document).keypress(function(e){
            if(e.keyCode==27){
                $("#background").fadeOut("slow");
                $("#preview").fadeOut("slow");
            }
        });
 
        $("#background").click(function(){
            $("#background").fadeOut("slow");
            $("#preview").fadeOut("slow");
        });
        
        $("#preview").click(function(){
            $("#background").fadeOut("slow");
            $("#preview").fadeOut("slow");
        });
    });
</script>   