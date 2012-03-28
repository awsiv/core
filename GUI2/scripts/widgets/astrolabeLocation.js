(function($) {
    $.widget('ui.astrolabeLocation', {
        options: {
            defaultbehaviour: true,
            defaultLabel: 'Selected: ',
            defaultHostLabel: 'Selected Host: ',
            defaultContextPathLabel: 'Selected Context: ',
            defaultValue: '(none)'
        },

        _create: function() {
            var $self = this;

            $self.element.addClass('astrolabeLocation');

            $self._label = $('<span>');
            $self._label.addClass('label');
            $self._label.html($self.options.defaultLabel);
            $self.element.append($self._label);

            $self._value = $('<span>');
            $self._value.addClass('label');
            $self._value.html($self.options.defaultValue);
            $self.element.append($self._value);
        },

        setContextPath: function(path, count) {
            $self = this;

            $self._label.html($self.options.defaultContextPathLabel);

            var pathString = '';
            var firstPath = true;
            for (var i in path) {
                if (!firstPath)
                    pathString = pathString + '->';
                pathString = pathString + path[i];
                firstPath = false;
            }

            if (count !== 0 && count !== undefined) {
                pathString = pathString + ' (' + count + ')';
            }

            $self._value.html(pathString);
        },

        setHostName: function(hostname) {
            $self = this;

            $self._label.html($self.options.defaultHostLabel);
            $self._value.html(hostname);
        },

        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }

    });

    $.extend($.ui.astrolabeLocation, {
        instances: []
    });
})(jQuery);
