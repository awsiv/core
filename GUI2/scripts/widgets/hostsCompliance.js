(function ($) {
    $.widget('ui.hostsCompliance', {
        options: {
            astrolabeUrl: '../astrolabe/',
            defaultbehaviour: true
        },

        _create: function () {
            var $self = this;
            
            $self.element.addClass('hostsCompliance');
            $self.element.addClass('container_12');

            $self._red = $self._createColourEntry('red');
            $self._red.addClass('grid_2');
            $self.element.append($self._red);
            
            $self._green = $self._createColourEntry('green');
            $self._green.addClass('grid_2');
            $self.element.append($self._green);
            
            $self.element.append($self._createClear());
            
            $self._yellow = $self._createColourEntry('yellow');
            $self._yellow.addClass('grid_2');
            $self.element.append($self._yellow);
            
            $self._blue = $self._createColourEntry('blue');
            $self._blue.addClass('grid_2');
            $self.element.append($self._blue);
            
            $self.element.append($self._createClear());
            
        },
        
        setContextPath: function(includes, excludes) {
            var $self = this;
            
            $.getJSON($self._requestUrls.hostCount($self, includes, 'blue'), 
                function(count) {
                    $self._setHostCount($self._blue, count);
                });
            $.getJSON($self._requestUrls.hostCount($self, includes, 'red'), 
                function(count) {
                    $self._setHostCount($self._red, count);
                });
            $.getJSON($self._requestUrls.hostCount($self, includes, 'green'), 
                function(count) {
                    $self._setHostCount($self._green, count);
                });
            $.getJSON($self._requestUrls.hostCount($self, includes, 'yellow'), 
                function(count) {
                    $self._setHostCount($self._yellow, count);
                });
        },
        
        _createClear: function() {
            var $element = $('<div>');

            $element.addClass('clear');
            
            return $element;
        },

        _createColourEntry: function(colour) {
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
            
            $self._setHostCount($entry, 0);
            
            return $entry;
        },

        _setHostCount: function($entry, count) {
            var $entryLabel = $entry.children('.colourEntryLabel');
            
            $entryLabel.html(count + ' hosts known');
        },
        
        _requestUrls: {
            
            hostCount: function(self, includes, colour) {

                return self.options.astrolabeUrl + 'hostcount/' +
                    'colour/' + colour + '/' +
                    'includes/' + encodeURIComponent(includes);
            }
        }
    });

    $.extend($.ui.hostsCompliance, {
        instances: []
    });

})(jQuery);