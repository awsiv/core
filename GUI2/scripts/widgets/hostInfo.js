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
                release: $self._createValueElement(),
                lastReportUpdate: $self._createValueElement(),
                lastPolicyUpdate: $self._createValueElement()
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
            
            $self.element.append($self._createLabelElement('Flavour'));
            $self.element.append($self._values.flavour);
            $self.element.append($self._createClear());
            
            $self.element.append($self._createLabelElement('Release'));
            $self.element.append($self._values.release);
            $self.element.append($self._createClear());
            
            $self.element.append($self._createLabelElement('Last Report'));
            $self.element.append($self._values.lastReportUpdate);
            $self.element.append($self._createClear());
            
            $self.element.append($self._createLabelElement('Policy Updated'));
            $self.element.append($self._values.lastPolicyUpdate);
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
            $element.addClass('grid_2');
            $element.html(label);
            
            return $element;
        },
        
        _createValueElement: function() {
            var $element = $('<span>');

            $element.addClass('value');
            $element.addClass('grid_3');
            $element.html('&nbsp;');
            
            return $element;
        },
        
        _clearFields: function() {
            var $self = this;
            
            $self._values.name.html('unknown');
            $self._values.ip.html('unknown');
            $self._values.osType.html('unknown');
            $self._values.flavour.html('unknown');
            $self._values.release.html('unknown');
            $self._values.lastReportUpdate.html('unknown');
            $self._values.lastPolicyUpdate.html('unknown');
        },
        
        updateHostKey: function(hostKey) {
            var $self = this;
        
            var requestUrl = $self.options.baseUrl + '/host/info/' + hostKey;
            
            $.getJSON(requestUrl, function(host) {
                $self._clearFields();
                $self._values.name.html(host.hostname);
                $self._values.ip.html(host.ip);
                
                if (host.osType !== undefined) {
                    $self._values.osType.html(host.osType);
                }
                if (host.flavour !== undefined) {
                    $self._values.flavour.html(host.flavour);
                }
                if (host.release !== undefined) {
                    $self._values.release.html(host.release);
                }
                
                if (host.lastReportUpdate === -1) {
                    $self._values.lastReportUpdate.html('never');
                }
                else if (host.lastReportUpdate !== undefined && host.lastReportUpdate >= 0) {
                    // TODO: print unix time for now, need to agree on formatting
                    $self._values.lastReportUpdate.html(host.lastReportUpdate);
                }
                
                if (host.lastPolicyUpdate !== undefined) {
                    $self._values.lastPolicyUpdate.html(host.lastPolicyUpdate);
                }
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