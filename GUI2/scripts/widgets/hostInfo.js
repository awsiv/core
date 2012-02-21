(function($) {
    $.widget('ui.hostInfo', {
        options: {
            baseUrl: '',
            defaultbehaviour: true
        },

        _create: function() {
            var $self = this;
    
            $self.element.addClass('hostInfo');
            $self.element.addClass('container_12');
            
            $self._values = {
                name: $self._createValueElement(),
                ip: $self._createValueElement(),
                osType: $self._createValueElement(),
                flavour: $self._createValueElement(),
                key: $self._createValueElement()
            };
            
            $self.element.append($self._createLabelElement('Name'));
            $self.element.append($self._values.name);
            $self.element.append($self._createClear());
            
            $self.element.append($self._createLabelElement('IP'));
            $self.element.append($self._values.ip);
            $self.element.append($self._createClear());
            
            $self.element.append($self._createLabelElement('OS Type'));
            $self.element.append($self._values.osType);
            $self.element.append($self._createClear());
            
            $self.element.append($self._createLabelElement('Key'));
            $self.element.append($self._values.key);
            $self.element.append($self._createClear());
            
            $.ui.hostInfo.instances.push($self.element);
        },
        
        _createClear: function() {
            var $element = $('<div>');

            $element.addClass('clear');
            
            return $element;
        },
        
        _createLabelElement: function(label) {
            var $element = $('<span>');

            $element.addClass('label');
            $element.addClass('grid_1');
            $element.html(label);
            
            return $element;
        },
        
        _createValueElement: function() {
            var $element = $('<span>');

            $element.addClass('value');
            $element.addClass('grid_2');
            $element.html('&nbsp;');
            
            return $element;
        },
        
        _clearFields: function() {
            var $self = this;
            
            $self._values.name.html('unknown');
            $self._values.ip.html('unknown');
            $self._values.osType.html('unknown');
            $self._values.key.html('unknown');
        },
        
        updateHostKey: function(hostKey) {
            var $self = this;
        
            var requestUrl = $self.options.baseUrl + '/host/info/' + hostKey;
            
            $.getJSON(requestUrl, function(host) {
                $self._clearFields();
                $self._values.name.html(host.hostname);
                $self._values.ip.html(host.ip);
                $self._values.osType.html(host.osType);
                $self._values.key.html(host.hostkey);
            });
        },
        
        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }
    });
    
    $.extend($.ui.hostInfo, {
        instances: []
    });
})(jQuery);