(function($) {
    $.widget('ui.hostsComplianceTimeseries', {
        options: {
            baseUrl: '../',
            defaultbehaviour: true,

            plot: {
                series: {
                    stack: true,
                    bars: {
                        show: true,
                        lineWidth: 0,
                        fill: 1,
                        align: 'center'
                    },
                    lines: {
                        show: false,
                        fill: false,
                        steps: false,
                        lineWidth: 0
                    },
                    shadowSize: 0
                },
                legend: {
                    show: false
                },
                xaxis: {
                    show: true,
                    color: common.colours.white,
                    mode: 'time'
                },
                yaxis: {
                    show: true,
                    color: common.colours.white,
                    minTickSize: 1,
                    min: 0,
                    tickDecimals: 0
                },
                grid: {
                    hoverable: true,
                    clickable: true,
                    autoHighlight: true
                },
                colors: [
                    common.colours.green,
                    common.colours.yellow,
                    common.colours.red
                ]
            }
        },

        _create: function() {
            var $self = this;

            $self.element.addClass('hostsComplianceTimeseries');

            var $header = $('<div>');
            $header.addClass('header');
            $self.element.append($header);

            $self._$loader = $('<span>');
            $self._$loader.addClass('loader');
            $header.append($self._$loader);

            $self._$graph = $('<div>');
            $self._$graph.css('height', '200px');
            $self.element.append($self._$graph);

            $shortTermRadio = $('<input>');
            $shortTermRadio.attr('type', 'radio');
            $shortTermRadio.attr('name', 'timeMode');
            $shortTermRadio.attr('value', 'short');
            $shortTermRadio.attr('id', 'timeModeShort')
            $shortTermRadio.click(function() {
                $self._resource = $self.options.baseUrl + '/host/compliance/timeseries';
                $self.options.plot.xaxis.tickFormatter = function(value) {
                    return common.time.formatTimeOfDay(value);
                };
                $self.refresh();
            });
            $self.element.append($shortTermRadio);
            $shortTermRadio.attr('checked', true);

            $shortTermLabel = $('<label>');
            $shortTermLabel.attr('for', 'timeModeShort');
            $shortTermLabel.html('Last 6 Hours');
            $self.element.append($shortTermLabel);

            $longTermRadio = $('<input>');
            $longTermRadio.attr('type', 'radio');
            $longTermRadio.attr('name', 'timeMode');
            $longTermRadio.attr('value', 'long');
            $longTermRadio.attr('id', 'timeModeLong');
            $longTermRadio.click(function() {
                $self._resource = $self.options.baseUrl + '/host/compliance/timeseries_shifts';
                $self.options.plot.xaxis.tickFormatter = function(value) {
                    return common.time.formatDate(value);
                };
                $self.refresh();
            });
            $self.element.append($longTermRadio);

            $longTermLabel = $('<label>');
            $longTermLabel.attr('for', 'timeModeLong');
            $longTermLabel.html('Last Week');
            $self.element.append($longTermLabel);

        },

        _init: function() {
            var $self = this;

            $self._plot = null;
            $self._previousPoint = null;
            $self._sampleCounts = null;
            $self._resource = $self.options.baseUrl + '/host/compliance/timeseries';
            $self.options.plot.xaxis.tickFormatter = function(value) {
                return common.time.formatTimeOfDay(value);
            };

            $self._$graph.bind('plothover', function(event, pos, item) {
                $('#x').text(pos.x.toFixed(2));
                $('#y').text(pos.y.toFixed(2));

                if (item) {
                    if ($self._previousPoint != item.dataIndex) {
                        $self._previousPoint = item.dataIndex;

                        $('#tooltip').remove();

                        var start = item.datapoint[0];
                        var end = start + $self._resolution;

                        var green = $self._plot.getData()[0].data[item.dataIndex][1];
                        var yellow = $self._plot.getData()[1].data[item.dataIndex][1];
                        var red = $self._plot.getData()[2].data[item.dataIndex][1];

                        $self._showTooltip(item.pageX, item.pageY,
                            'Interval: ' + common.time.formatTimeOfDay(start) + ' to ' +
                                common.time.formatTimeOfDay(end) + '<br/>' +
                            'Green Hosts: ' + green + '<br/>' +
                            'Yellow Hosts: ' + yellow + '<br/>' +
                            'Red Hosts: ' + red + '<br/>');
                    }
                }
                else {
                    $('#tooltip').remove();
                    $self._previousPoint = null;
                }
            });

            $self._$graph.bind('plotclick', function (event, pos, item) {
                if (item) {
                    $self._onGraphClick($self, item);
                }
            });

            // change cursor on hover
            $self._$graph.bind("plothover", function(event, pos, item) {
                if(item) {
                    document.body.style.cursor = 'pointer';
                } else {
                    document.body.style.cursor = 'default';
                }
            });
            
            $self._$loader.hide();
        },

        setContext: function(includes, excludes, totalHostCount) {
            var $self = this;

            $self._context.includes = includes;
            $self._context.excludes = excludes;

            $self.options.plot.yaxis.max = totalHostCount;

            $self.refresh();
        },

        setHost: function(hostKey) {
            var $self = this;

            $self._context.includes = [common.canonify('PK_' + hostKey)];
            $self._context.excludes = [];

            $self.options.plot.yaxis.max = 1;

            $self.refresh();
        },

        refresh: function() {
            var $self = this;

            $self._$loader.show();

            $.ajax({
                url: $self._requestUrls.timeseries($self),
                dataType: 'json',

                success: function(timeseries) {
                    var green = common.arrayWithValue(0, timeseries.count);
                    var red = common.arrayWithValue(0, timeseries.count);
                    var yellow = common.arrayWithValue(0, timeseries.count);
                    $self._resolution = common.unixTimeToJavascriptTime(timeseries.resolution);
                    var i = 0;

                    var convertToTimeDomain = function(positionSeries) {
                        var timeEntry = function(position) {
                            return common.unixTimeToJavascriptTime(timeseries.from) +
                            (position * common.unixTimeToJavascriptTime(timeseries.resolution));
                        };

                        var timeArray = [];
                        for (var pos = 0; pos < positionSeries.length; pos++) {
                            timeArray.push([timeEntry(pos), positionSeries[pos]]);
                        }
                        return timeArray;
                    }

                    for (i = 0; i < timeseries.data.length; i++) {
                        var entry = timeseries.data[i];

                        green[entry.position] = entry.green;
                        red[entry.position] = entry.red;
                        yellow[entry.position] = entry.yellow;
                    }

                    green = convertToTimeDomain(green);
                    red = convertToTimeDomain(red);
                    yellow = convertToTimeDomain(yellow);

                    $self.options.plot.series.bars.barWidth = $self._resolution -
                        (0.05 * $self._resolution);

                    $self._plot = $.plot($self._$graph, [
                        {
                            label: 'Green Hosts',
                            data: green
                        },
                        {
                            label: 'Yellow Hosts',
                            data: yellow
                        },
                        {
                            label: 'Red Hosts',
                            data: red
                        }
                    ], $self.options.plot);
                },

                complete: function() {
                    $self._$loader.hide();
                }
            });
        },

        _onGraphClick: function($self, item) {
            var from = item.datapoint[0];
            var to = from + $self._resolution;

            var includes = '';
            if ($self._context.includes.length > 0) {
                includes = '/includes/' + encodeURIComponent($self._context.includes);
            }

            var element = $('<a href="' + $self.options.baseUrl + '/widget/summaryCompliance' +
                '/start/'+ common.javascriptTimeToUnixTime(from) +
                '/stop/' + common.javascriptTimeToUnixTime(to) +
                includes +
                '" title="Overview : Promises not kept or repaired ( ' +
                common.time.format(from) +
                ' - ' +
                common.time.format(to) +
                ' )" />');
            var option = {'width':'1025','height':'445'};
            element.ajaxyDialog(option).ajaxyDialog("open");
        },

        _showTooltip: function(x, y, contents) {
            $('<div>').attr('id', 'tooltip').addClass('tooltip').css({
                top: y + 5,
                left: x + 5
            }).html(contents).appendTo('body').fadeIn(100);
        },

        _context: {
            includes: [],
            excludes: []
        },

        _requestUrls: {
            timeseries: function($self) {
                var url = $self._resource + '?';

                if ($self._context.includes.length > 0) {
                    url = url + 'includes=' + encodeURIComponent($self._context.includes) + '&';
                }

                if ($self._context.excludes.length > 0) {
                    url = url + 'excludes=' + encodeURIComponent($self._context.excludes);
                }

                return url;
            }
        }
    });

    $.extend($.ui.hostsCompliance, {
        instances: []
    });

})(jQuery);
