var hostsMeter  = {
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
        var self = this;
        
        var json = self.options.data;      
        
        var barChartOptions = {
            //id of the visualization container
            injectInto: self.element.attr('id'),
            //whether to add animations
            animate: false,//this.options.animate,
            type: self.options.useGradients? 'stacked:gradient' : 'stacked',
            //labels style
            Label: {
                type: self.options.labelType, //Native or HTML
                size: 13,
                family: 'Arial',
                color: 'white'
            }

        };

        $.extend(barChartOptions, self.options.barChartOptions);

        self._barChart = new $jit.BarChart(barChartOptions);
        self._barChart.loadJSON(json);
    },
    
    updateHostKey: function(hostKey) {
        var self = this;
        
        var requestUrl = self.options.baseUrl + '/astrolabe/meters/' + hostKey;
        
        $.getJSON(requestUrl, function(json) {
            self._barChart.loadJSON({
                    color: ['#779a62','#f0d76a','#cc4358'],
                    'label': json.labels,
                    'values': json.values
                });
        });
    },
    
    options: {
        data: '',
        labelType:'',
        useGradients:false,
        nativeTextSupport:'',
        animate: true,        
        barChartOptions : {
            //horizontal or vertical 
            orientation: 'vertical',
            //bars separation
            barsOffset: 20,
            //visualization offset
            Margin: {
                top:5,
                left: 15,
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
$.widget("ui.hostsMeter", hostsMeter);