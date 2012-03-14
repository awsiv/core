(function($) {
    $.widget('ui.hostsCompliance', {
        options: {
            baseUrl: '../',
            defaultbehaviour: true
        },

        _create: function() {
            var $self = this;

            $self.element.addClass('hostsCompliance');

            $self._red = $self._createColourEntry('red');
            $self.element.append($self._red);

            $self._green = $self._createColourEntry('green');
            $self.element.append($self._green);

            $self._yellow = $self._createColourEntry('yellow');
            $self.element.append($self._yellow);
        },

        setContext: function(includes, excludes) {
            var $self = this;

            $self._context.includes = includes;
            $self._context.excludes = excludes;

            $self.refresh();
        },

        refresh: function() {
            var $self = this;

            $.getJSON($self._requestUrls.hostCount($self, $self._context.includes, 'red'),
                function(count) {
                    $self._setHostCount($self._red, count, '> 20% not compliant');
                });
            $.getJSON($self._requestUrls.hostCount($self, $self._context.includes, 'green'),
                function(count) {
                    $self._setHostCount($self._green, count, '> 80% compliant');
                });
            $.getJSON($self._requestUrls.hostCount($self, $self._context.includes, 'yellow'),
                function(count) {
                    $self._setHostCount($self._yellow, count, '> 20% repaired, now compliant');
                });
        },

        _createColourEntry: function(colour, info) {
            var $self = this;

            var $entry = $('<div>');
            $entry.addClass('colourEntry');

            var $entryIcon = $('<span>');
            $entryIcon.addClass('colourEntryIcon');
            $entryIcon.addClass(colour);
            $entryIcon.html('&nbsp;');
            $entry.append($entryIcon);

            var $entryLabel = $('<span>');
            $entryLabel.addClass('colourEntryLabel');
            $entry.append($entryLabel);

            $self._setHostCount($entry, 0, info);

            return $entry;
        },

        _setHostCount: function($entry, count, info) {
            var $entryLabel = $entry.children('.colourEntryLabel');

            $entryLabel.html(count + ' hosts (' + info + ')');
        },

        _context: {
            includes: [],
            excludes: []
        },

        _requestUrls: {

            hostCount: function(self, includes, colour) {

                return self.options.baseUrl + 'host/count?' +
                    'colour=' + colour + '&' +
                    'includes=' + encodeURIComponent(includes);
            }
        }
    });

    $.extend($.ui.hostsCompliance, {
        instances: []
    });

})(jQuery);
