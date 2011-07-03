var hostMeter  = {
    _create: function() {
        // creation code for performance analysis graph
        var ua = navigator.userAgent,
        iStuff = ua.match(/iPhone/i) || ua.match(/iPad/i),
        typeOfCanvas = typeof HTMLCanvasElement,
        nativeCanvasSupport = (typeOfCanvas == 'object' || typeOfCanvas == 'function'),
        textSupport = nativeCanvasSupport
        && (typeof document.createElement('canvas').getContext('2d').fillText == 'function');
        //I'm setting this based on the fact that ExCanvas provides text support for IE
        //and that as of today iPhone/iPad current text support is lame
        this.options.labelType = (!nativeCanvasSupport || (textSupport && !iStuff))? 'Native' : 'HTML';
        this.options.nativeTextSupport = this.options.labelType == 'Native';
        this.options.animate = !(iStuff || !nativeCanvasSupport);
    },
    _init: function() {
        this.initialize();
    },
    initialize: function() {
        var json = this.options.data;      
        
        var barChartOptions = {
            //id of the visualization container
            injectInto: this.element.attr('id'),
            //whether to add animations
            animate: this.options.animate,
            type: this.options.useGradients? 'stacked:gradient' : 'stacked',
            //labels style
            Label: {
                type: this.options.labelType, //Native or HTML
                size: 13,
                family: 'Arial',
                color: 'black'
            }

        };

        $.extend(barChartOptions,this.options.barChartOptions);
        var barChart = new $jit.BarChart(barChartOptions);
        barChart.loadJSON(json);
        
    },
    options: { // initial values are stored in the widget's prototype
        data: '',
        labelType:'',
        useGradients:false,
        nativeTextSupport:'',
        animate:'',        
        barChartOptions : {
            //horizontal or vertical barcharts
            orientation: 'vertical',
            //bars separation
            barsOffset: 20,
            //visualization offset
            Margin: {
                top:5,
                left: 10,
                right: 5,
                bottom:5
            },
            //labels offset position

            labelOffset: 5,
            //bars style
            //whether to show the aggregation of the values
            showAggregates:false,
            //whether to show the labels for the bars
            showLabels:true,
            Tips: {
                enable: true,
                onShow: function(tip, elem) {
                    tip.innerHTML =  elem.name + " " + parseFloat(elem.value).toFixed(2) + "%";
                }
            }
        }
    }
        
};
$.widget("ui.hostMeter", hostMeter); // create the widget           