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
                    mode: 'time',
                    timeformat: '%b-%d',
                    minTickSize: [1, 'day']
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
        },

        _init: function() {
            var $self = this;

            $self._plot = null;
            $self._previousPoint = null;
            $self._sampleCounts = null;
            $self._hostCounts = null;

            $self.element.bind('plothover', function(event, pos, item) {
                $('#x').text(pos.x.toFixed(2));
                $('#y').text(pos.y.toFixed(2));

                if (item) {
                    if ($self._previousPoint != item.dataIndex) {
                        $self._previousPoint = item.dataIndex;

                        $('#tooltip').remove();

                        var start = item.datapoint[0];
                        var end = start + $self._resolution;

                        var kept = $self._plot.getData()[0].data[item.dataIndex][1];
                        var repaired = $self._plot.getData()[1].data[item.dataIndex][1];
                        var notkept = $self._plot.getData()[2].data[item.dataIndex][1];

                        var hostCount = $self._hostCounts[item.dataIndex];
                        var sampleCount = $self._sampleCounts[item.dataIndex];

                        var percentageKept = ((kept * 100) / hostCount).toFixed(2);
                        var percentageRepaired = ((repaired * 100) / hostCount).toFixed(2);
                        var percentageNotKept = ((notkept * 100) / hostCount).toFixed(2);

                        $self._showTooltip(item.pageX, item.pageY,
                            'Interval: ' + common.time.formatTimeOfDay(start) + ' to ' +
                                common.time.formatTimeOfDay(end) + '<br/>' +
                            'Promises Kept: ' + percentageKept + '%<br/>' +
                            'Promises Repaired: ' + percentageRepaired + '%<br/>' +
                            'Promises Not Kept: ' + percentageNotKept + '%<br/>' +
                            'Number of Measurements: ' + sampleCount + '<br/>' +
                            'Number of Reporting Hosts: ' + hostCount + '<br/>');
                    }
                }
                else {
                    $('#tooltip').remove();
                    $self._previousPoint = null;
                }
            });
        },

        setContext: function(includes, excludes) {
            var $self = this;

            $self._context.includes = includes;
            $self._context.excludes = excludes;

            $self.refresh();
        },

        setHost: function(hostKey) {
            var $self = this;

            $self._context.includes = [common.canonify('PK_' + hostKey)];
            $self._context.excludes = [];

            $self.refresh();
        },

        refresh: function() {
            var $self = this;

            $.getJSON($self._requestUrls.timeseries($self),
                function(timeseries) {

                    var kept = common.arrayWithValue(0, timeseries.count);
                    var notkept = common.arrayWithValue(0, timeseries.count);
                    var repaired = common.arrayWithValue(0, timeseries.count);
                    $self._sampleCounts = common.arrayWithValue(0, timeseries.count);
                    $self._hostCounts = common.arrayWithValue(0, timeseries.count);
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

                        kept[entry.position] = (entry.kept / 100) * entry.hostcount;
                        notkept[entry.position] = (entry.notkept / 100) * entry.hostcount;
                        repaired[entry.position] = (entry.repaired / 100) * entry.hostcount;
                        $self._sampleCounts[entry.position] = entry.samplecount;
                        $self._hostCounts[entry.position] = entry.hostcount;
                    }

                    kept = convertToTimeDomain(kept);
                    notkept = convertToTimeDomain(notkept);
                    repaired = convertToTimeDomain(repaired);

                    $self.options.plot.series.bars.barWidth = $self._resolution -
                        (0.05 * $self._resolution);

                    $self.options.plot.yaxis.max = Math.max.apply(Math, $self._hostCounts);

                    $self._plot = $.plot($self.element, [
                        {
                            label: 'Promises Kept',
                            data: kept
                        },
                        {
                            label: 'Promises Repaired',
                            data: repaired
                        },
                        {
                            label: 'Promises Not Kept',
                            data: notkept
                        }
                    ], $self.options.plot);
                });
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
                var url = $self.options.baseUrl + 'host/compliance/timeseries?';

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
