(function($) {
    $.widget('ui.vitalSigns', {
        options: {
            contextWidgetElement:'hclist',
            baseUrl:'',
            vitalsurl: '/visual/getNodeVitals',
        },
        _context: {
            includes: [],
            excludes: []
        },
        $busyIcon: $('<span class="loadinggif" style="display:none">')
        .html('&nbsp;'),
        $errorDiv: $('<div>').addClass('error'),

        _create: function() {
            var $self = this;
            $self.refresh();
        },

        setContext: function(includes, excludes) {
            var $self = this;
            $self._context.includes = includes;
            $self._context.excludes = excludes;
            $self._modifyContext();
        },

        _modifyContext:function () {
            var $self = this;
            var includes = encodeURIComponent($self._context.includes);
            var excludes = encodeURIComponent($self._context.excludes);
            $self.clearCanvas();
            $self.refresh();

        },
        clearCanvas:function() {
            var $self = this;
            $self.element.html('');
        },

        refresh:function() {
            var self = this;
            var params = {
                'url':  self.options.baseUrl + self.options.vitalsurl,
                'data': {

                },
                'success': function (data) {
                    self.drawGraphs(data);
                }
            };
            self.sendRequest(params);
        },

        plotGraph: function(meta,perfdata) {

            // create div
            var $self = this;
            var $hostLabel = $('<div>').html(meta.hostname);
            var intRegex = /^\d+$/;
            var lastUpdateDate = intRegex.test(meta.lastUpdated)   ? common.time.formatDate(common.unixTimeToJavascriptTime(meta.lastUpdated)) : 'unknown';
            var $lastUpdated = $('<div>').html('Last updated: '+ lastUpdateDate );
            var $graphHeader = $('<div class="graphHeader">').append($hostLabel)
            .append($lastUpdated);
            var $graphDiv = $('<div style="float:left;margin:10px;">').attr('id',meta.hostkey+'-container').append($graphHeader)

            var $plotdiv = $('<div>').attr('id',meta.hostkey).width('250').height('150');
            $graphDiv.append($plotdiv);
            $self.element.append($graphDiv);

            var data = perfdata;
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
                    data:data,
                    lines: {
                        show:true
                    },
                    color:"rgb(255, 153, 0)"
                },
                ]
                ,options);




        },

        drawGraphs: function (data) {
            var self = this;
            //console.log(data);
            $.each(data, function(key, value) {
                var perfdata = (value.perfdata);
                var meta = value.meta;
                self.plotGraph(meta,perfdata);
            });
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
