(function($) {
    $.widget('ui.hostsConnectivity', {
        options: {
            baseUrl: '../',
            defaultbehaviour: true
        },

        _create: function() {
            var $self = this;

            $self.element.addClass('hostsConnectivity');

            $self._blue = $self._createColourEntry('blue',
                'Hosts that have not been reached (their state is unknown)');
            $self.element.append($self._blue);

            $self._black = $self._createColourEntry('black',
                'Hosts that show an agent schedule failure. (Host skipped its last 3 scheduled executions)');
            $self.element.append($self._black);

            $self.element.append($self._createClear());
        },

        _init: function() {
            var $self = this;

            $.getJSON($self._requestUrls.blueHostThreshold($self),
                function(blueHostThreshold) {
                    $self._blue.children('.colourEntryLabel').attr('title',
                        'Hosts that have not been reached ' +
                        '(their state is unknown) for more than: ' +
                        blueHostThreshold / 60 + ' minutes');
                });
        },

        setContext: function(includes, excludes) {
            var $self = this;

            $self._context.includes = includes;
            $self._context.excludes = excludes;

            $self.refresh();
        },

        refresh: function() {
            var $self = this;

            $.getJSON($self._requestUrls.hostCount($self, $self._context.includes, 'blue'),
                function(count) {
                    $self._setHostCount($self._blue, count);
                });
            $.getJSON($self._requestUrls.hostCount($self, $self._context.includes, 'black'),
                function(count) {
                    $self._setHostCount($self._black, count);
                });
        },

        _createClear: function() {
            var $element = $('<div>');

            $element.addClass('clear');

            return $element;
        },

        _createColourEntry: function(colour, tooltip) {
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
            $entryLabel.attr('title', tooltip);
            $entryLabel.click(function () {
                window.location.href = $self.options.baseUrl + '/welcome/hosts/' +
                    colour + '/' + encodeURIComponent($self._context.includes) + '/' +
                    encodeURIComponent($self._context.excludes);
            });
            $entry.append($entryLabel);

            $self._setHostCount($entry, 0);

            return $entry;
        },

        _setHostCount: function($entry, count) {
            var $entryLabel = $entry.children('.colourEntryLabel');

            $entryLabel.html(count + ' hosts unreachable');
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

            blueHostThreshold: function(self) {

                return self.options.baseUrl + '/appsettings/bluehostthreshold';
            }
        }
    });

    $.extend($.ui.hostsCompliance, {
        instances: []
    });

})(jQuery);
