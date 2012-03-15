(function($) {
    $.widget('ui.hostsCompliance', {
        options: {
            baseUrl: '../',
            defaultbehaviour: true
        },

        _create: function() {
            var $self = this;

            $self.element.addClass('hostsCompliance');

            $self._red = $self._createColourEntry('red', '> 20% not compliant',
                'More than 20% of promises could not be kept at last measurement.');
            $self.element.append($self._red);

            $self._green = $self._createColourEntry('green', '> 80% compliant',
                'More than 80% of promises are stable and kept.');
            $self.element.append($self._green);

            $self._yellow = $self._createColourEntry('yellow', '> 20% repaired, now compliant',
                'More than 20% of promises needed repair at last measurement.');
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

        _createColourEntry: function(colour, label, tooltip) {
            var $self = this;

            var $entry = $('<div>');
            $entry.addClass('colourEntry');

            var $entryIcon = $('<span>');
            $entryIcon.addClass('colourEntryIcon');
            $entryIcon.addClass(colour);
            $entryIcon.html('&nbsp;');
            $entry.append($entryIcon);

            var $entryLabel = $('<a>');
            $entryLabel.addClass('colourEntryLabel');
            $entryLabel.addClass('showqtip');
            $entryLabel.attr('title', tooltip)
            $entryLabel.click(function () {
                window.location.href = $self.options.baseUrl + '/welcome/host/' +
                    colour + '/' + encodeURIComponent($self._context.includes) + '/' +
                    encodeURIComponent($self._context.excludes);
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
