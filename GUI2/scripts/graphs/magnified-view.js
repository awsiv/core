var magnifiedView  = {
    _create: function() {
        var magnifiedViewdata = <?php echo ($graphdata); ?>;
        var lineSeries1 = <?php echo ($graphdatalineseries1); ?>;
        var lineSeries2 = <?php echo ($graphdatalineseries2); ?>;
      
    },
    _init: function() {
        this.initialize();
    },
    initialize: function() {
            
        

        var plot = $.plot($("#placeholder-magview_<?php echo $observable; ?>"),
        [
            { data:magnifiedViewdata, magnifiedView: {show: true, lineWidth: 1}},
            { data:lineSeries1,lines: {show:true},color:"rgb(255, 153, 0)"},
            { data:lineSeries2,lines: {show: true, lineWidth: 1},color:"rgb(0, 204, 0)"}
        ]
        ,options);

    },
    options: { // initial values are stored in the widget's prototype
        
    }        
};
$.widget("ui.magnifiedView", magnifiedView); // create the widget