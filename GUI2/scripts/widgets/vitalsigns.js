(function($) {
    $.widget('ui.vitalSigns', {
        options: {
            contextWidgetElement: 'hclist',
            baseUrl: '',
            interval: 0, // Interval in minutes to refresh the widget
            defaultNumberOfGraphs : 10,
            vitalsurl: '/vitals/node_vitals',
            vitalsurlForHost: '/vitals/host_vitals',
            vitalsurlForConfiguration: '/vitals/config_vitals',
            vitalsMainLink: '/visual/vital/'
        },
        _context: {
            includes: [],
            excludes: []
        },
        busyIcon: $('<span class="loadinggif" style="display:none">').html('&nbsp;'),
        showMoreIcon: $('<span class="showmore button"  style="display:none">').html('<button class="button">show more...</button>'),
        errorDiv: $('<div style="display:none">').addClass('error'),

        _startIndex: 0,

        _cachedData: [],
        _hostView: false,

        vitalPanel: $('<div class="graph-container-header-top-menu"></div>' +
            '<div class="loading-div"></div><div class="graph-container-body"></div>' +
            '<div class="graph-container-footer-menu"></div>'),

        vitalsSelect: $('<select id="vitalsSelect">'),
        vitalsSortBySelect: $('<select id="vitalsSortBySelect">'),
        vitalsOrderBySelect: $('<select id="vitalsSortBySelect">'),
        selectVitalsValues: {
            'loadavg': 'load average',
            'diskfree': 'Disk free',
            'cpu': 'CPU (all)'
        },
        vitalsValuesSortBy: {
            'last-measured': 'Last Measured',
            'average': 'Average',
            'slope': 'Slope'
        },

        selectedVital: 'loadavg',
        selectedVitalSortBy: 'last-measured',
        selectedVitalOrderBy: 'desc',

        _timer: null,
        _vitalConfig: null,


        _create: function() {
            var $self = this;
            $self.element.append($self.vitalPanel);
            $self._fetchConfig();
            $self.element.find('.loading-div').prepend($self.busyIcon);
            $self.element.find('.loading-div').prepend($self.errorDiv);
            $self._setTimer(this.options.interval);
        },

        setContext: function(includes,excludes,draw) {
            var $self = this;
            $self._modifyContext(includes, excludes);
            $self._hostView = false;
            if (draw) {
                $self.refresh();
            }
        },

        _setTimer: function(value) {
            this.options.interval = value;
            // Clear the old timer
            window.clearInterval(this._timer);
            if (value > 0) {
                // Set a new timer based on the new interval
                this._timer = window.setInterval(
                    $.proxy(this.refresh, this),
                    this.options.interval * 60 * 1000
                    );
            }
        },

        _setOption: function(key, value) {
            if (this.options[key] === value) {
                return this; // Do nothing, the value didn't change
            }
            if (key === 'interval') {
                this._setTimer(value);
            }
            return this;
        },

        _modifyContext: function(includes,excludes) {
            var $self = this;
            $self._context.includes = includes;
            $self._context.excludes = excludes;
        },

        setHostContext: function(hostkey,draw) {
            var $self = this,
            includes = [common.canonify('PK_' + hostkey)],
            excludes = [];
            $self._hostView = true;
            $self._modifyContext(includes, excludes);
            if (draw) {
                $self.refresh();
            }
        },

        refresh: function() {
            var $self = this;
            $self.clearCanvas();
            if ($self._hostView === false) {
                $self.refreshForNodeView();
            } else {
                $self.refreshForHostView();
            }
        },

        clearCanvas: function() {
            var $self = this;
            $self.busyIcon.hide();
            $self.errorDiv.hide();
            $self.element.find('.graph-container-body').empty();
            $self.element.find('.graph-container-footer-menu').empty();
            $self.resetCounter();
        },

        resetCounter: function() {
            var $self = this;
            $self._startIndex = 0;
            $self._cachedData = [];
        },

        _buildVitalsSelection: function(data) {
            var $self = this;
            $.each(data.vitals, function(key, value) {
                $self.vitalsSelect
                .append($('<option></option>')
                    .attr('value', key)
                    .text(value.text));
            });
            $self.vitalsSelect.change(function() {
                $self.selectedVital = ($(this).val());
                $self.clearCanvas();
                $self.refreshForNodeView();
            });
            $self.element.find('.graph-container-header-top-menu').append($self.vitalsSelect);
            // build the vitals value drop down
            $self._buildVitalsSortSelection(data['vitals-sort-by']);
        },

        _buildVitalsSortSelection: function(sortBy) {
            //build the vitals sort by drop down

            var $self = this;
            var sortByValues = !$.isEmptyObject(sortBy) ? sortBy : $self.vitalsValuesSortBy;
            $.each(sortByValues, function(key, value) {
                $self.vitalsSortBySelect
                .append($('<option></option>')
                    .attr('value', key)
                    .text(value));
            });
            $self.vitalsSortBySelect.change(function() {
                $self.selectedVitalSortBy = ($(this).val());
                $self.clearCanvas();
                $self.refreshForNodeView();
            });

            var $orderByValues = {"desc":"Descending","asc":"Ascending"};
            $.each($orderByValues, function(key, value) {
                $self.vitalsOrderBySelect
                .append($('<option></option>')
                    .attr('value', key)
                    .text(value));
            });
            $self.vitalsOrderBySelect.change(function() {
                $self.selectedVitalOrderBy = ($(this).val());
                $self.clearCanvas();
                $self.refreshForNodeView();
            });


            $self.element.find('.graph-container-header-top-menu').append($self.vitalsSortBySelect).append($self.vitalsOrderBySelect);
        },

        _fetchConfig: function() {
            var $self = this;
            var params = {
                'url': $self.options.baseUrl + $self.options.vitalsurlForConfiguration,
                'type': 'get',
                'success': function(data) {
                    $self._vitalConfig = data;
                    $self._buildVitalsSelection(data);
                },
                'error': function() {
                    $self._buildVitalsSelection($self.selectVitalsValues);
                }
            };
            $self.sendRequest(params);
        },

        showEmptyData: function() {
            // cannot fetch any data
            var self = this;
            var $infoDiv = $('<p class=info>Cannot fetch any data right now.</p>');
            self.element.find('.graph-container-body').append($infoDiv);
        },

        refreshForNodeView: function() {
            var self = this;
            //self.busyIcon.show();
            var params = {
                'url': self.options.baseUrl + self.options.vitalsurl,
                'type': 'Get',
                'data': {
                    'obs': self.selectedVital,
                    'sort': self.selectedVitalSortBy,
                    'sortOrder': self.selectedVitalOrderBy
                },
                'success': function(data) {
                    if (data === null || data.length === 0) {
                        self.showEmptyData();
                        return;
                    }
                    self._cachedData = data;
                    self.drawGraphCanvas(self._cachedData);
                //self.busyIcon.hide();
                }
            };
            self.sendRequest(params);
        },

        refreshForHostView: function() {
            var self = this;
            self.element.find('.graph-container-header-top-menu').hide();
            self.busyIcon.show();
            var params = {
                'url': self.options.baseUrl + self.options.vitalsurlForHost,
                'type': 'Get',
                'data': {

                },
                'success': function(data) {
                    if (data === null || data.length === 0) {
                        self.showEmptyData();
                        return;
                    }
                    self.drawGraphCanvasForHost(data);
                    self.busyIcon.hide();
                }
            };
            self.sendRequest(params);

        },

        plotGraph: function($plotdiv,meta,perfdata) {

            var data = perfdata;
            var length = data.length;
            var lastDataSeries = [];
            var averageSeries = [];
            var maxArrayValue = 0; // for scaling the y axis max value
            for (i = 0; i < length; i++) {
                if (data[i][1] > maxArrayValue) {
                    maxArrayValue = data[i][1];
                }
                lastDataSeries.push([i, data[i][1]]);
                averageSeries.push([i, data[i][2]]);
            }

            var ymin = null;
            var ymax = null;
            if (('obs' in meta) && this._vitalConfig.vitals[meta.obs] !== undefined) {
                var configYmax = this._vitalConfig.vitals[meta.obs].max ? this._vitalConfig.vitals[meta.obs].max : null;
                var configYmin = this._vitalConfig.vitals[meta.obs].min ? this._vitalConfig.vitals[meta.obs].min : null;
                ymax = configYmax > maxArrayValue ? this._vitalConfig.vitals[meta.obs].max : null;
                ymin = configYmin;
            }


            var options = {
                grid: {
                    hoverable: true,
                    clickable: true,
                    axisMargin: 15,
                    labelMargin: 15
                },
                yaxis: {
                    min: ymin,
                    max: ymax
                },
                xaxis: {
                    max: 48,
                    tickSize: 12,
                    tickFormatter: function(val, axis) {

                        var subtract = (4 - (val / 12)) * 60 * 60;

                        var actualDate = parseInt(meta.lastUpdated, 10);
                        if (!actualDate) return val;
                        var displayDate = actualDate - subtract;
                        var d = new Date();
                        d.setTime(displayDate * 1000);
                        return d.getHours() + ' : ' + d.getMinutes();


                    }
                }
            };

            var plot = $.plot($plotdiv,
                [
                {
                    label: '',
                    data: lastDataSeries,
                    lines: {
                        show: true
                    },
                    color: 'rgb(255, 153, 0)'
                },

                {
                    label: '',
                    data: averageSeries,
                    lines: {
                        show: true,
                        lineWidth: 1
                    },
                    color: 'rgb(0, 204, 0)'
                }
                ]
                , options);

            var previousPoint = null;
            $plotdiv.bind('plothover', function(event, pos, item) {
                if (item) {
                    if (previousPoint != item.datapoint) {
                        previousPoint = item.datapoint;

                        $('#vital-sign-tooltip').remove();
                        var x = item.datapoint[0].toFixed(2),
                        y = item.datapoint[1].toFixed(2);
                        showTooltip(item.pageX, item.pageY, y);
                    }
                }
                else {
                    $('#vital-sign-tooltip').remove();
                    previousPoint = null;
                }
            });

            function showTooltip(x, y, contents) {
                $('<div id="vital-sign-tooltip">' + contents + '</div>').css({
                    position: 'absolute',
                    display: 'none',
                    top: y + 5,
                    left: x + 5,
                    border: '1px solid #fdd',
                    padding: '10px',
                    color: '#000',
                    'background-color': '#fee',
                    opacity: 0.80
                }).appendTo('body').fadeIn(100);
            }




        },

        drawGraphCanvas: function(data) {
            var self = this;
            self.element.find('.graph-container-header-top-menu').show();
            var offset = self._startIndex * self.options.defaultNumberOfGraphs;
            var slicedData = data.slice(offset, offset + self.options.defaultNumberOfGraphs);
            $.each(slicedData, function(key, value) {
                var perfdata = (value.perfdata);
                var meta = value.meta;
                meta['obs'] = self.selectedVital;

                // create div
                var $url = self.options.baseUrl + self.options.vitalsMainLink + meta.hostkey + '/' + self.selectedVital;
                var $hostVitalLink = $('<a target="_blank">').html(meta.hostname).attr('href', $url);
                var $hostLabel = $('<div>').addClass('graph-title').append($hostVitalLink);
                var intRegex = /^\d+$/;
                var lastUpdateDate = intRegex.test(meta.lastUpdated) ? common.time.formatDate(common.unixTimeToJavascriptTime(meta.lastUpdated)) : 'unknown';
                var $lastUpdated = $('<div>').html('Last updated: ' + lastUpdateDate);
                var lastMeasuredValue = (perfdata.length > 0) ? perfdata[perfdata.length - 1][1] : 'not available';
                var $lastMeasuredValueDiv = $('<div>').html('Last measured value: ' + lastMeasuredValue);
                var $graphHeader = $('<div class="graphHeader">').append($hostLabel)
                .append($lastUpdated).append($lastMeasuredValueDiv);
                var $graphDiv = $('<div>').addClass('graph-container').attr('id', meta.hostkey + '-container').append($graphHeader);

                var $plotdiv = $('<div>').attr('id', meta.hostkey).width('250').height('150').html('loading...');
                $graphDiv.append($plotdiv);
                self.element.find('.graph-container-body').append($graphDiv);
                self.plotGraph($plotdiv, meta, perfdata);
            });

            if ((data.length > self.options.defaultNumberOfGraphs) && (self._startIndex * self.options.defaultNumberOfGraphs + self.options.defaultNumberOfGraphs) < data.length) {
                // show load more button
                self.showLoadMoreButton();
            } else {
                self.hideLoadMoreButton();
            }

        },

        drawGraphCanvasForHost: function(data) {

            var self = this;
            if (data.length !== 0) {
                var meta = {
                    'hostname': data.hostname,
                    'lastUpdated': data.ls,
                    'hostkey': data.hostkey
                };
                $.each(data.obs, function(key, value) {
                    var perfdata = (value.perfdata);
                    if (perfdata) {
                        // create div
                        var $url = self.options.baseUrl + self.options.vitalsMainLink + meta.hostkey + '/' + value.id;
                        var $hostVitalLink = $('<a target="_blank">').html(value.id + '<br />' + value.desc).attr('href', $url);
                        var $hostLabel = $('<div>').addClass('graph-title').append($hostVitalLink);
                        var intRegex = /^\d+$/;
                        var lastUpdateDate = intRegex.test(meta.lastUpdated) ? common.time.formatDate(common.unixTimeToJavascriptTime(meta.lastUpdated)) : 'unknown';
                        var $lastUpdated = $('<div class"graph-last-updated">').html('Last updated: ' + lastUpdateDate);
                        var $unit = $('<div class"graph-unit">').html('Unit: ' + value.units);
                        var $graphHeader = $('<div class="graphHeader">').append($hostLabel)
                        .append($lastUpdated).append($unit);
                        var $graphDiv = $('<div>').attr('id', value.id + '-container').addClass('graph-container').append($graphHeader);

                        var $plotdiv = $('<div>').attr('id', value.id).width('250').height('150');
                        $graphDiv.append($plotdiv);
                        self.element.find('.graph-container-body').append($graphDiv);
                        self.plotGraph($plotdiv, meta, perfdata);
                    }
                });



                if ((data.length > self.options.defaultNumberOfGraphs) && (self._startIndex * self.options.defaultNumberOfGraphs + self.options.defaultNumberOfGraphs) < data.length) {
                    // show load more button
                    self.showLoadMoreButton();
                } else {
                    self.hideLoadMoreButton();
                }
            }
        },

        showLoadMoreButton: function() {
            var $self = this;
            $self.element.find($self.showMoreIcon).remove();
            $self.showMoreIcon.show();
            $self.showMoreIcon.click(function() {
                $self._startIndex++;
                $self.drawGraphCanvas($self._cachedData);
            });
            $self.element.find('.graph-container-footer-menu').append($self.showMoreIcon);
        },

        hideLoadMoreButton: function() {
            var $self = this;
            $self.showMoreIcon.hide();
            $self.element.find($self.showMoreIcon).remove();
        },

        _getContext: function() {
            var self = this;
            var context = [];
            context['includes'] = encodeURIComponent(self._context.includes);
            context['excludes'] = encodeURIComponent(self._context.excludes);
            return context;
        },

        sendRequest: function(params) {
            var self = this;
            var senddata = $.extend(params.data, self._getContext());
            $.ajax({
                beforeSend: function() {
                    self.busyIcon.show();
                },
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
                complete: function() {
                    self.busyIcon.hide();
                },
                error: function(jqXHR, textStatus, errorThrown) {
                    if ($.isFunction(params.error)) {
                        return $.call(params.error());
                    }
                    var excludeCodes = [401, 403, 404, 500, 0];

                    if ($.inArray(jqXHR.status, excludeCodes) === -1) {
                        self.showError('Cannot fetch data due to unknown error.');
                    }

                },
                statusCode: {
                    '401': function() {
                        self.showError('Access denied.');
                        return;
                    },
                    '403': function() {
                        self.showError('Unauthorized access.');
                        return;
                    },
                    '404': function() {
                        self.showError('Cannot fetch data as supplied url is not found.');
                        return;
                    },
                    '500': function() {
                        self.showError('Invalid server response.');
                        return;
                    },
                    '0': function() {
                        self.showError('Cannot contact the server.');
                        return;
                    }
                }
            });
        },
        showError: function(text) {
            this.errorDiv.html(text).show();
        },
        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }

    });

    $.extend($.ui.vitalSigns, {
        instances: []
    });
})(jQuery);
