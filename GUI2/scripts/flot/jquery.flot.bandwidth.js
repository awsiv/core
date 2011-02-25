/*
 * The MIT License

Copyright (c) 2010 by Juergen Marsch

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/*
Flot plugin for bandwidth data sets

  series: {
    bandwidth: { active: true, lineWidth: 8
    ,drawBandwidth: function(ctx, bandwidth, x, y1, y2, color) { alert(color); }
  }
data: [

  $.plot($("#placeholder"), [{ data: [ [y, x, size], [....], ...], bandwidth: {show: true, lineWidth: 5} } ])

*/

(function ($)
{
    var options =

    {
        series:

        {
            bandwidth:

            {
                active: false
                ,
                show: false
                ,
                fill: true
                ,
                lineWidth: 4
                ,
                highlight: {
                    opacity: 0.5
                }
                ,
                drawBandwidth: drawBandwidthDefault
            }
        }
        ,
        yaxis: {
            min: 0
        }
    };
    function drawBandwidthDefault(ctx,bandwidth, x,y1,y2,color)
    {
        ctx.beginPath();
        ctx.strokeStyle = color;
        ctx.lineWidth = bandwidth.lineWidth;
        ctx.lineCap = "round";
        ctx.moveTo(x, y1);
        ctx.lineTo(x, y2);
        ctx.stroke();
    }
    var  data = null, canvas = null, target = null, axes = null, offset = null, highlights = [];
    function init(plot)
    {
        plot.hooks.processOptions.push(processOptions);
        function processOptions(plot,options)
        {
            if(options.series.bandwidth.active)

            {
                plot.hooks.draw.push(draw);
                plot.hooks.bindEvents.push(bindEvents);
                plot.hooks.drawOverlay.push(drawOverlay);
            }
        }
        function draw(plot, ctx)
        {
            var series;
            canvas = plot.getCanvas();
            target = $(canvas).parent();
            axes = plot.getAxes();
           
            offset = plot.getPlotOffset();
            data = plot.getData();
            for (var i = 0; i < data.length; i++)
            {
                series = data[i];
                if(series.bandwidth.show)
                {
                    for (var j = 0; j < series.data.length; j++)

                    {
                        drawBandwidth(ctx,series.data[j],series.bandwidth,series.color,false);
                    }
                }
            }
        }
        function drawBandwidth(ctx,data,bandwidth,color,isOverlay)
        {
            var x,y1,y2;
            x = offset.left + axes.xaxis.p2c(data[0]);
          
            y1 = offset.top + axes.yaxis.p2c(data[1]);
            y2 = offset.top + axes.yaxis.p2c(data[2]);
            bandwidth.drawBandwidth(ctx,bandwidth,x,y1,y2,color,isOverlay);
        }
        function bindEvents(plot, eventHolder)
        {
            var options = plot.getOptions();
            var hl = new HighLighting(plot, eventHolder, findNearby, options.series.bandwidth.active,highlights)
        }
        function findNearby(plot,mousex, mousey)
        {
            var series,r;
            data = plot.getData();
            axes = plot.getAxes()
            for(var i = 0;i < data.length;i++)
            {
                series = data[i];
                if(series.bandwidth.show)
                {
                    for(var j = 0; j < series.data.length; j++)

                    {
                        var x,y1,y2,dataitem;
                        dataitem = series.data[j];
                        x = axes.xaxis.p2c(dataitem[0]) - series.bandwidth.lineWidth / 2;
                        
                        y1 = axes.yaxis.p2c(dataitem[1]);
                        y2 = axes.yaxis.p2c(dataitem[2]);
                        if (mousex >= x && mousex <= (x + series.bandwidth.lineWidth))
                        {
                            if (y1 < y2)

                            {
                                if (mousey > y1 && mousey < y2) {
                                    r = {
                                        i: i,
                                        j: j
                                    };
                            }
                        }
                    else
                    {
                        if (mousey > y2 && mousey < y1) {
                            r = {
                                i: i,
                                j:j
                            };

                    }
                    }
            }
            }
    }
}
return r;
}
function drawOverlay(plot, octx)
{
    octx.save();
    octx.clearRect(0, 0, target.width(), target.height());
    for(i = 0; i < highlights.length; ++i)
    {
        drawHighlight(highlights[i]);
    }
    octx.restore();
    function drawHighlight(s)
    {
        var c = "rgba(255, 255, 255, " + s.series.bandwidth.highlight.opacity + ")";
        drawBandwidth(octx,s.point,s.series.bandwidth,c,true);
    }
}
}
$.plot.plugins.push({
    init: init,
    options: options,
    name: 'bandwidth',
    version: '0.4'
});
})(jQuery);