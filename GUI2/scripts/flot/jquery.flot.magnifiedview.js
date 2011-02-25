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
Flot plugin for magnifiedview data sets

  series: {
    magnifiedView: { active: true, lineWidth: 8
    ,drawMagnifiedView: function(ctx, magnifiedView, x, y1, y_av,delta, color) { alert(color); }
  }
data: [

  $.plot($("#placeholder"), [{ data: [ [x, y1,y_av,delta], [....], ...], magnifiedView: {show: true, lineWidth: 5} } ])

 */

(function ($)
{
    var pluginOptions =

        {
        series:

            {
            magnifiedView:

                {
                active: false,
                lastUpdated : '',
                show: false,
                fill: true,
                lineWidth: 4 ,
                highlight: {
                    opacity: 0.5
                },
                drawMagnifiedView: drawMagnifiedViewDefault
            }
        }
        
    };
    function drawMagnifiedViewDefault(ctx,magnifiedView, x,y1,y_av,delta,color,isOverlay)
    {
        //console.log(x,y1,y_av,delta,color);
        ctx.lineWidth = magnifiedView.lineWidth;
        ctx.beginPath();

       

        midpoint = y_av;
        colorUp = '#FF0000';
        colorDown = '#00FF00';

        ctx.strokeStyle = colorUp;

        var mid = offset.top + axes.yaxis.p2c(midpoint);
        var y22 = offset.top + axes.yaxis.p2c(midpoint+delta);
        x = offset.left + axes.xaxis.p2c(x);

        ctx.moveTo(x,mid);
        ctx.lineTo(x,y22);
        ctx.stroke();

        ctx.beginPath();
        ctx.strokeStyle = colorDown;
        var y11= offset.top + axes.yaxis.p2c(midpoint-delta);
        ctx.moveTo(x, y11);
        ctx.lineTo(x, mid);
        ctx.stroke();

        ctx.beginPath();
        if (isOverlay)
        {
            ctx.strokeStyle = "rgba(255,255,255," + magnifiedView.highlight.opacity + ")";
        }
        else
        {
            ctx.strokeStyle = "#f80";
        }
       
    }
    var  data = null, canvas = null, target = null, axes = null, offset = null, highlights = [];
    function init(plot)
    {
        plot.hooks.processOptions.push(processOptions);
        function processOptions(plot,options)
        {
            //console.log(options);
            if(options.series.magnifiedView.active)
            {
                plot.hooks.draw.push(draw);
                // plot.hooks.bindEvents.push(bindEvents);
                //plot.hooks.drawOverlay.push(drawOverlay);

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
                if(series.magnifiedView.show)
                {
                    for (var j = 0; j < series.data.length; j++)

                    {
                        drawMagnifiedView(ctx,series.data[j],series.magnifiedView,series.color,false);
                    }
                }
            }
        }
        function drawMagnifiedView(ctx,data,magnifiedView,color,isOverlay)
        {
            var x,y1,y_av,delta;
            x = (data[0]);

            y1 = (data[1]);
            y_av = (data[2]);
            delta = (data[3]);
            magnifiedView.drawMagnifiedView(ctx,magnifiedView,x,y1,y_av,delta,color,isOverlay);
        }
        function bindEvents(plot, eventHolder)
        {
            var options = plot.getOptions();
            var hl = new HighLighting(plot, eventHolder, findNearby, options.series.magnifiedView.active,highlights)
        }
        function findNearby(plot,mousex, mousey)
        {/*
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
 */
        }

    


    }
    $.plot.plugins.push({
        init: init,
        options: pluginOptions,
        name: 'magnifiedView',
        version: '0.1'
    });
})(jQuery);