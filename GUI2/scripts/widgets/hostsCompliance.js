(function($) {
    $.widget('ui.hostsCompliance', {
        options: {
            baseUrl: '../',
            defaultbehaviour: true
        },
        ajaxloader:$('<span class="loadinggif"></span>'),
        header:$('<p class="title">Host Compliance (last hour)</p>'),
        _create: function() {
            var $self = this;

           
            $self.element.append($self.header);
            $self.ajaxloader.appendTo($self.header);
            $self.ajaxloader.hide();
            $self._red = $self._createColourEntry('red', '> 20% not compliant',
                'More than 20% of promises could not be kept at last measurement.');
            $self.element.append($self._red);

            $self._yellow = $self._createColourEntry('yellow', '> 20% repaired, now compliant',
                'More than 20% of promises needed repair at last measurement.');
            $self.element.append($self._yellow);
            
            $self._green = $self._createColourEntry('green', '> 80% compliant',
                'More than 80% of promises are stable and kept.');
            $self.element.append($self._green);
            
        },

        setContext: function(includes, excludes) {
            var $self = this;

            $self._context.includes = includes;
            $self._context.excludes = excludes;

            $self.refresh();
        },

        refresh: function() {
            var $self = this;
            $self.ajaxloader.show();
            $self.element.find('.colourEntryLabel').html('');
            $.getJSON($self._requestUrls.hostCountAll($self, $self._context.includes),
                function(data) {
                    $self._setHostCount($self._red, data.red, '> 20% not compliant');
                    $self._setHostCount($self._yellow, data.yellow, '> 20% repaired, now compliant');
                    $self._setHostCount($self._green, data.green, '> 80% compliant');
                    $self.ajaxloader.hide();
             }).error(function(jqXHR,textStatus, errorThrown){
                    $self.ajaxloader.hide();
             });

        },

        _createColourEntry: function(colour, label, tooltip) {
            var $self = this;

            var $entry = $('<div>');
            $entry.addClass('colourEntry');

            var $entryIcon = $('<span>');
            $entryIcon.addClass('colourEntryIcon '+colour).html('&nbsp;').appendTo($entry);

            var $entryLabel = $('<a>');
            $entryLabel.addClass('colourEntryLabel showqtip').attr('title', tooltip).data({my:'left center', at:'right center'})
            $entryLabel.click(function () {
                var redirectUrl=$self.options.baseUrl + '/welcome/hosts/' +colour ;
                if($self._context.includes.length > 0){
                    redirectUrl +='/includes/' + encodeURIComponent($self._context.includes) 
                }
                if($self._context.excludes.length > 0){
                    redirectUrl+='/excludes/' + encodeURIComponent($self._context.excludes);
                }
                window.location.href = redirectUrl;
            });
            $entry.append($entryLabel);

            $self._setHostCount($entry, 0, label);

            return $entry;
        },

        _setHostCount: function($entry, count, label) {
            var $entryLabel = $entry.children('.colourEntryLabel');
            $entryLabel.html(count + ' hosts (' + label + ')');
        },

        _context: {
            includes: [],
            excludes: []
        },

        _requestUrls: {

            hostCount: function(self, includes, colour) {

                return self.options.baseUrl + '/host/count?' +
                    'colour=' + colour + '&' +
                    'includes=' + encodeURIComponent(includes);
            },
            
            hostCountAll:function(self,includes){
                 return self.options.baseUrl + '/host/countCompliance?' +'includes=' + encodeURIComponent(includes);
            }
            
        }
    });

    $.extend($.ui.hostsCompliance, {
        instances: []
    });

})(jQuery);
