(function ($) {
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
                    shadowSize:  0
                },
                legend: {
                    show: false
                },
                xaxis: {
                    show: true,
                    color: common.colours.white,
                    mode: "time",
                    timeformat: "%b-%d",
                    minTickSize: [1, "day"]
                },
                yaxis: {
                    show: true,
                    color: common.colours.white,
                    minTickSize: 1,
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

        _create: function () {
            var $self = this;

            $self.element.addClass('hostsComplianceTimeseries');

        },

        setContext: function(includes, excludes) {
            var $self = this;

            $self._context.includes = includes;
            $self._context.excludes = excludes;

            $self.refresh();
        },

        refresh: function() {
            var $self = this;

            $.getJSON($self._requestUrls.timeseries($self),
                function(timeseries) {

                    var kept = common.arrayWithValue(0, timeseries.count);
                    var notkept = common.arrayWithValue(0, timeseries.count);
                    var repaired = common.arrayWithValue(0, timeseries.count);
                    var i = 0;

                    var convertToTimeDomain = function(positionSeries) {
                        var timeEntry = function (position) {
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
                    }

                    kept = convertToTimeDomain(kept);
                    notkept = convertToTimeDomain(notkept);
                    repaired = convertToTimeDomain(repaired);

                    $self.options.plot.series.bars.barWidth =
                        common.unixTimeToJavascriptTime(timeseries.resolution);

                    $.plot($self.element, [
                    {
                        label: "Promises Kept",
                        data: kept
                    },
                    {
                        label: "Promises Repaired",
                        data: repaired
                    },
                    {
                        label: "Promises Not Kept",
                        data: notkept
                    }
                    ], $self.options.plot);
                });
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