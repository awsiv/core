  	/*
Flot plugin for stacking data sets, i.e. putting them on top of each
other, for accumulative graphs.

The plugin assumes the data is sorted on x (or y if stacking
horizontally). For line charts, it is assumed that if a line has an
undefined gap (from a null point), then the line above it should have
the same gap - insert zeros instead of "null" if you want another
behaviour. This also holds for the start and end of the chart. Note
that stacking a mix of positive and negative values in most instances
doesn't make sense (so it looks weird).

Two or more series are stacked when their "stack" attribute is set to
the same key (which can be any number or string or just "true"). To
specify the default stack, you can set

  series: {
    stack: null or true or key (number/string)
  }

or specify it for a specific series

  $.plot($("#placeholder"), [{ data: [ ... ], stack: true }])

The stacking order is determined by the order of the data series in
the array (later series end up on top of the previous).

Internally, the plugin modifies the datapoints in each series, adding
an offset to the y value. For line series, extra data points are
inserted through interpolation. If there's a second y value, it's also
adjusted (e.g for bar charts or filled areas).
*/

(function ($) {
    var options = {
        series: { stack: null, // or number/string
        waterfall: null, // or number/string  waterfall, fixedColor, positivColor and negativColor are Waterfall -Params
				 fixedColor: "#808080",
				 positivColor: "#00c000",
				 negativColor: "#c00000",
				 yaxisAuto: null
				}
    };

    function init(plot) {
        function findMatchingSeries(s, allseries) {
            var res = null
            for (var i = 0; i < allseries.length; ++i) {
                if (s == allseries[i])
                    break;

                if (allseries[i].stack == s.stack)
                    res = allseries[i];
            }

            return res;
        }
        function waterfallData(plot, series, data, datapoints) { //Hookup function for waterfall
			if(series.waterfall == null){
				return;
			}
			else {
				var data = plot.getData();
				var options = plot.getOptions(); //alertObject(options);
				if (data.length == 1) {  //convert rawdata only, dont convert already converted data
					var x = convertWaterfallToStack(data, series);
					plot.setData(x.data);
					if(options.series.yaxisAuto) { options.yaxis.min = x.yaxismin * options.series.yaxisAuto; }
					waterfallTicks(data,options); //convert ticks based on rawdata
				}
			}
		}
		function waterfallTicks(data,options){
			if (options.ticks == null) {	//Ticks are not predefined by options
				var ticks = options.xaxis.ticks = [];
				var dataSeries = data[0].data;
				for (var i = 0; i < dataSeries.length; i++) {
					ticks.push([i + 0.3, dataSeries[i][0]]);
				}
			}
		}
		function convertWaterfallToStack(data, series){ //convert waterfalldata to 4 Bars, d1 for fixed bars, d2 for invisible bar
			var d1 = [], d2 = [], d3 = [], d4 = [], p = 0, mn = Number.POSITIVE_INFINITY;  // d3 for negative bars and d4 for positive bars
		  	var dx = data[0].data;
	      	for (var i = 0; i < dx.length; i++) {
		    	if (dx[i][2]){
					if(typeof(dx[i][1]) == "undefined") {
						d1.push([i,p]);
					}
					else {
						d1.push([i,dx[i][1]]);
						p = dx[i][1];
					}
		    	}
		    	else {
			  		d1.push([i,0]);
			  		if(dx[i][1] > 0) {
			 			d4.push([i,- dx[i][1]]);p = p + dx[i][1];d2.push([i,p]);d3.push([i,0]);
			  		}
			  		else {
						d3.push([i,- dx[i][1]]);p = p + dx[i][1];d2.push([i,p]);d4.push([i,0]);
			  		}
		    	}
				mn = Math.min(mn,p);
			}  //rebuild data
          	var dr = { data: [ { data: d1, color: series.fixedColor }
			                  ,{ data: d2, bars: { show: false }, lines: { show: false } }
				              ,{ data: d3, color: series.negativColor }
				              ,{ data: d4, color: series.positivColor}
							 ]
						,yaxismin: mn
					};
	      	return dr;
	    }

        function stackData(plot, s, datapoints) {
            if (s.stack == null)
                return;

            var other = findMatchingSeries(s, plot.getData());
            if (!other)
                return;

            var ps = datapoints.pointsize,
                points = datapoints.points,
                otherps = other.datapoints.pointsize,
                otherpoints = other.datapoints.points,
                newpoints = [],
                px, py, intery, qx, qy, bottom,
                withlines = s.lines.show,
                horizontal = s.bars.horizontal,
                withbottom = ps > 2 && (horizontal ? datapoints.format[2].x : datapoints.format[2].y),
                withsteps = withlines && s.lines.steps,
                fromgap = true,
                keyOffset = horizontal ? 1 : 0,
                accumulateOffset = horizontal ? 0 : 1,
                i = 0, j = 0, l;

            while (true) {
                if (i >= points.length)
                    break;

                l = newpoints.length;

                if (points[i] == null) {
                    // copy gaps
                    for (m = 0; m < ps; ++m)
                        newpoints.push(points[i + m]);
                    i += ps;
                }
                else if (j >= otherpoints.length) {
                    // for lines, we can't use the rest of the points
                    if (!withlines) {
                        for (m = 0; m < ps; ++m)
                            newpoints.push(points[i + m]);
                    }
                    i += ps;
                }
                else if (otherpoints[j] == null) {
                    // oops, got a gap
                    for (m = 0; m < ps; ++m)
                        newpoints.push(null);
                    fromgap = true;
                    j += otherps;
                }
                else {
                    // cases where we actually got two points
                    px = points[i + keyOffset];
                    py = points[i + accumulateOffset];
                    qx = otherpoints[j + keyOffset];
                    qy = otherpoints[j + accumulateOffset];
                    bottom = 0;

                    if (px == qx) {
                        for (m = 0; m < ps; ++m)
                            newpoints.push(points[i + m]);

                        newpoints[l + accumulateOffset] += qy;
                        bottom = qy;

                        i += ps;
                        j += otherps;
                    }
                    else if (px > qx) {
                        // we got past point below, might need to
                        // insert interpolated extra point
                        if (withlines && i > 0 && points[i - ps] != null) {
                            intery = py + (points[i - ps + accumulateOffset] - py) * (qx - px) / (points[i - ps + keyOffset] - px);
                            newpoints.push(qx);
                            newpoints.push(intery + qy);
                            for (m = 2; m < ps; ++m)
                                newpoints.push(points[i + m]);
                            bottom = qy;
                        }

                        j += otherps;
                    }
                    else { // px < qx
                        if (fromgap && withlines) {
                            // if we come from a gap, we just skip this point
                            i += ps;
                            continue;
                        }

                        for (m = 0; m < ps; ++m)
                            newpoints.push(points[i + m]);

                        // we might be able to interpolate a point below,
                        // this can give us a better y
                        if (withlines && j > 0 && otherpoints[j - otherps] != null)
                            bottom = qy + (otherpoints[j - otherps + accumulateOffset] - qy) * (px - qx) / (otherpoints[j - otherps + keyOffset] - qx);

                        newpoints[l + accumulateOffset] += bottom;

                        i += ps;
                    }

                    fromgap = false;

                    if (l != newpoints.length && withbottom)
                        newpoints[l + 2] += bottom;
                }

                // maintain the line steps invariant
                if (withsteps && l != newpoints.length && l > 0
                    && newpoints[l] != null
                    && newpoints[l] != newpoints[l - ps]
                    && newpoints[l + 1] != newpoints[l - ps + 1]) {
                    for (m = 0; m < ps; ++m)
                        newpoints[l + ps + m] = newpoints[l + m];
                    newpoints[l + 1] = newpoints[l - ps + 1];
                }
            }

            datapoints.points = newpoints;
        }
        plot.hooks.processRawData.push(waterfallData);
        plot.hooks.processDatapoints.push(stackData);
    }

    $.plot.plugins.push({
        init: init,
        options: options,
        name: 'stack',
        version: '1.2, waterfall 0.2'
    });
})(jQuery);