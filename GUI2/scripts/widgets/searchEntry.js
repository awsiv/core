(function($) {
    $.widget('ui.searchEntry', {
        options: {
            defaultbehaviour: true
        },

        _create: function() {
            var $self = this;

            $self.element.addClass('searchEntry');
            var $searchInput = $('<input>');
            $searchInput.addClass('searchInput');
            $searchInput.bind('keypress', function() {
                setTimeout(function() {
                    $self._trigger('onChanged', null, { value: $searchInput.val() });
                }, 1);
            });
            $self.element.append($searchInput);

            var $searchButton = $('<span>');
            $searchButton.addClass('searchButton');
            $searchButton.html('&nbsp;');
            $self.element.append($searchButton);
        },

        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }
    });

    $.extend($.ui.combobox, {
        instances: []
    });
})(jQuery);
