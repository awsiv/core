(function($) {
    $.widget('ui.vitalSigns', {
        options: {
            contextWidgetElement:'hclist',
            baseUrl:'',
            vitalsurl: '/visual/getNodeVitals',
            vitalsurlForHost: '/visual/getHostVitals'

        },
        _context: {
            includes: [],
            excludes: []
        },
<<<<<<< HEAD
        $busyIcon: $('<span class="loadinggif" style="display:none">')
        .html('&nbsp;'),
        $errorDiv: $('<div>').addClass('error'),

=======
        busyIcon: $('<span class="loadinggif" style="display:none">').html('&nbsp;'),
        showMoreIcon: $('<span class="showmore"  style="display:none">').html('<button>show more..</button>'),
        errorDiv: $('<div>').addClass('error'),

        _startIndex:0,
        _defaultNumberOfGraphs:10,
        _cachedData:[],
        _hostView: false,

        vitalPanel: $('<div class="graph-container-header-top-menu"></div>'+
            '<div class="graph-container-body"></div>'+
            '<div class="graph-container-footer-menu"></div>'),

        vitalsSelect: $('<select id="vitalsSelect">'),
        selectVitalsValues : {
            "loadavg":"load average",
            'diskfree':"Disk free"
        },
        selectedVital : 'loadavg',


>>>>>>> added support for vital selection for nodes.
        _create: function() {
            var $self = this;
            $self.element.append($self.vitalPanel);
            $self.addNodeContexHeader();
        },

        setContext: function(includes, excludes) {
            var $self = this;
            $self._context.includes = includes;
            $self._context.excludes = excludes;
            $self._modifyContext();
            $self._hostView = false;
        },

        setHostContext:function(hostkey) {
            var $self = this;
            var includes = [common.canonify('PK_' + hostkey)];
            var excludes = [];
            $self._hostView = true;
            $self.setContext(includes,excludes);
        },

        _modifyContext:function () {
            var $self = this;
            $self.clearCanvas();
            if ($self._hostView === false) {
                $self.refreshForNodeView();
            } else {
                $self.refreshForHostView();
            }
        },

        clearCanvas:function() {
            var $self = this;
<<<<<<< HEAD
            $self.element.html('');
=======
            $self.element.find('.graph-container-body').empty();
            $self.element.find('.graph-container-footer-menu').empty();
            $self.resetCounter();
        },

        resetCounter: function() {
            var $self = this;
            $self._startIndex = 0;
            $self._defaultNumberOfGraphs=10;
            $self._cachedData=[];
>>>>>>> added support for vital selection for nodes.
        },

        addNodeContexHeader: function() {
            var $self = this;
            $.each($self.selectVitalsValues, function(key, value) {
                $self.vitalsSelect
                .append($("<option></option>")
                    .attr("value",key)
                    .text(value));
            });
            $self.vitalsSelect.change(function() {
                $self.selectedVital = ($(this).val());
                $self.element.find('.graph-container-body').empty();
                $self.element.find('.graph-container-footer-menu').empty();
                $self.resetCounter();
                $self.refreshForNodeView();
            });
            $self.element.find('.graph-container-header-top-menu').append( $self.vitalsSelect);
        },

        refreshForNodeView:function() {
            var self = this;
            var params = {
                'url':  self.options.baseUrl + self.options.vitalsurl,
                'data': {
                    'obs':self.selectedVital
                },
                'success': function (data) {
<<<<<<< HEAD
                    self.drawGraphs(data);
=======
                    self._cachedData = data;
                    self.drawGraphCanvas(self._cachedData);
>>>>>>> added support for vital selection for nodes.
                }
            };
            self.sendRequest(params);
        },

        refreshForHostView:function() {
            var self = this;
            var params = {
                'url':  self.options.baseUrl + self.options.vitalsurlForHost,
                'data': {

                },
                'success': function (data) {
                    self.drawGraphCanvasForHost(data);
                }
            };
            self.sendRequest(params);
        },

        plotGraph: function($plotdiv,meta,perfdata) {

            var data = perfdata;
            var length = data.length;
            var lastDataSeries = [];
            var averageSeries = [];
            for (i=0;i<length;i++) {
                lastDataSeries.push([i, data[i][1]]);
                averageSeries.push([i, data[i][2]]);
            }

            var options = {
                grid:   {
                    hoverable: true,
                    clickable: true,
                    axisMargin: 15,
                    labelMargin: 15
                },
                xaxis: {
                    max:48,
                    tickSize:12,
                    tickFormatter: function (val, axis) {

                        var subtract = (4-(val/12))*60*60;

                        var actualDate = parseInt(meta.lastUpdated,10);
                        if (!actualDate) return val;
                        var displayDate = actualDate -subtract;
                        var d = new Date();
                        d.setTime( displayDate * 1000 );
                        return d.getHours() + ' : ' + d.getMinutes();


                    }
                }
            };

            var plot = $.plot($plotdiv,
                [
                {
                    label:"",
                    data:lastDataSeries,
                    lines: {
                        show:true
                    },
                    color:"rgb(255, 153, 0)"
                },

                {
                    label:"",
                    data:averageSeries,
                    lines: {
                        show: true,
                        lineWidth: 1
                    },
                    color:"rgb(0, 204, 0)"
                }
                ]
                ,options);

            var previousPoint = null;
            $plotdiv.bind("plothover", function (event, pos, item) {
                if (item) {
                    if (previousPoint != item.datapoint) {
                        previousPoint = item.datapoint;

                        $("#vital-sign-tooltip").remove();
                        var x = item.datapoint[0].toFixed(2),
                        y = item.datapoint[1].toFixed(2);
                        showTooltip(item.pageX, item.pageY, y);
                    }
                }
                else {
                    $("#vital-sign-tooltip").remove();
                    previousPoint = null;
                }
            });

            function showTooltip(x, y, contents) {
                $('<div id="vital-sign-tooltip">' + contents + '</div>').css( {
                    position: 'absolute',
                    display: 'none',
                    top: y + 5,
                    left: x + 5,
                    border: '1px solid #fdd',
                    padding: '2px',
                    color: '#000',
                    'background-color': '#fee',
                    opacity: 0.80
                }).appendTo("body").fadeIn(100);
            }




        },

        drawGraphCanvas: function (data) {
            console.log('drawing node graphs..');

            var self = this;
<<<<<<< HEAD
            //console.log(data);
            $.each(data, function(key, value) {
=======
            self.element.find('.graph-container-header-top-menu').show();
            var offset = self._startIndex * self._defaultNumberOfGraphs;
            var slicedData = data.slice(offset, offset+self._defaultNumberOfGraphs);
            $.each(slicedData, function(key, value) {
>>>>>>> added support for vital selection for nodes.
                var perfdata = (value.perfdata);
                var meta = value.meta;

                // create div

                var $hostLabel = $('<div>').html(meta.hostname).addClass('graph-title');
                var intRegex = /^\d+$/;
                var lastUpdateDate = intRegex.test(meta.lastUpdated)   ? common.time.formatDate(common.unixTimeToJavascriptTime(meta.lastUpdated)) : 'unknown';
                var $lastUpdated = $('<div>').html('Last updated: '+ lastUpdateDate );
                var $graphHeader = $('<div class="graphHeader">').append($hostLabel)
                .append($lastUpdated);
                var $graphDiv = $('<div>').addClass('graph-container').attr('id',meta.hostkey+'-container').append($graphHeader)

                var $plotdiv = $('<div>').attr('id',meta.hostkey).width('250').height('150');
                $graphDiv.append($plotdiv);
                self.element.find('.graph-container-body').append($graphDiv);


                self.plotGraph($plotdiv,meta,perfdata);
            });
<<<<<<< HEAD
=======

            if ((data.length > self._defaultNumberOfGraphs) && (self._startIndex * self. _defaultNumberOfGraphs +  self. _defaultNumberOfGraphs ) <  data.length ) {
                // show load more button
                self.showLoadMoreButton();
            } else {
                self.hideLoadMoreButton();
            }

        },

        drawGraphCanvasForHost: function (data) {

            console.log('drawing host graphs..');
            var self = this;
            self.element.find('.graph-container-header-top-menu').hide();
            if (data.length !== 0) {
                var meta ={
                    'hostname':data.hostname,
                    'lastUpdated':data.ls
                };
                $.each(data.obs, function(key, value) {
                    var perfdata = (value.perfdata);
                    if (perfdata) {
                        // create div

                        var $hostLabel = $('<div>').html(value.id+'<br />'+value.desc).addClass('graph-title');
                        var intRegex = /^\d+$/;
                        var lastUpdateDate = intRegex.test(meta.lastUpdated)   ? common.time.formatDate(common.unixTimeToJavascriptTime(meta.lastUpdated)) : 'unknown';
                        var $lastUpdated = $('<div class"graph-last-updated">').html('Last updated: '+ lastUpdateDate );
                        var $unit = $('<div class"graph-unit">').html('Unit: '+ value.units );
                        var $graphHeader = $('<div class="graphHeader">').append($hostLabel)
                        .append($lastUpdated).append($unit);
                        var $graphDiv = $('<div>').attr('id',value.id+'-container').addClass('graph-container').append($graphHeader)

                        var $plotdiv = $('<div>').attr('id',value.id).width('250').height('150');
                        $graphDiv.append($plotdiv);
                        self.element.find('.graph-container-body').append($graphDiv);
                        self.plotGraph($plotdiv,meta,perfdata);
                    }
                });



                if ((data.length > self._defaultNumberOfGraphs) && (self._startIndex * self. _defaultNumberOfGraphs +  self. _defaultNumberOfGraphs ) <  data.length ) {
                    // show load more button
                    self.showLoadMoreButton();
                } else {
                    self.hideLoadMoreButton();
                }
            }


        },

        showLoadMoreButton:function() {
            var $self = this;
            $self.element.find( $self.showMoreIcon).remove();
            $self.showMoreIcon.show();
            $self.showMoreIcon.click(function() {
                $self._startIndex++;
                $self.drawGraphCanvas($self._cachedData);
            });
            $self.element.find('.graph-container-footer-menu').append( $self.showMoreIcon);
        },

        hideLoadMoreButton:function() {
            var $self = this;
            $self.showMoreIcon.hide();
            $self.element.find($self.showMoreIcon).remove();
>>>>>>> added support for vital selection for nodes.
        },

        sendRequest: function(params) {
            var self = this;
            var senddata = $.extend(params.data, self._context);
            $.ajax({
                type: params.type ? params.type : 'post' ,
                url: params.url,
                data: senddata,
                dataType: params.dataType ? params.dataType : 'json',
                success: function(data) {
                    if ($.isFunction(params.success)) {
                        return $.call(params.success(data));
                    }
                    return data;
                },
                error: function(jqXHR, textStatus, errorThrown) {
                    if ($.isFunction(params.error)) {
                        return $.call(params.error());
                    }
                    self._displayFailure(jqXHR,textStatus, errorThrown);
                }
            });
        },

        _displayFailure: function(jqXHR,textStatus, errorThrown) {
            var serverMsg,
            self = this;
            if (jqXHR.status == 0) {
                serverMsg = 'You are offline!!\n Please Check Your Network.';
            }else if (jqXHR.status == 404) {
                serverMsg = 'Requested URL not found.';
            }else if (jqXHR.status == 500) {
                serverMsg = 'Internel Server Error. ' + jqXHR.responseText;
            }else if (errorThrown == 'parsererror') {
                serverMsg = 'Error.\nParsing JSON Request failed.';
            }else if (errorThrown == 'timeout') {
                serverMsg = 'Request Time out.';
            }else {
                serverMsg = 'Unknow Error.\n' + jqXHR.responseText;
            }
            self.element.html("<div class='ui-state-error' style='padding: 1em;width:90%'><p><span style='float: left; margin-right: 0.3em;' class='ui-icon ui-icon-alert'></span>" + ' ' + serverMsg + '</p></div>');

        },



        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }

    });

    $.extend($.ui.vitalSigns, {
        instances: []
    });
})(jQuery);
