(function($) {
    $.widget('ui.hostInfo', {
        options: {
            baseUrl: '',
            defaultbehaviour: true
        },

        _create: function() {
            var $self = this;

            $self.element.addClass('hostInfo');

            $self._values = {
                ip: $self._createValueElement(),
                osType: $self._createValueElement(),
                flavour: $self._createValueElement(),
                release: $self._createValueElement(),
                lastReportUpdate: $self._createValueElement(),
                lastPolicyUpdate: $self._createValueElement()
            };

            var $header = $('<div>').addClass('header');
            {
                $self._colourIcon = $('<span>').addClass('colourIcon');
                $header.append($self._colourIcon);

                $self._headerLabel = $('<span>').addClass('headerLabel');
                $header.append($self._headerLabel);
            }
            $self.element.append($header);
            $self.element.append($self._createClear());

            var $column1 = $('<div>').addClass('column');
            {
                $column1.append($self._createLabelElement('IP'));
                $column1.append($self._values.ip);
                $column1.append($self._createClear());

                $column1.append($self._createLabelElement('OS Type'));
                $column1.append($self._values.osType);
                $column1.append($self._createClear());

                $column1.append($self._createLabelElement('Flavour'));
                $column1.append($self._values.flavour);
                $column1.append($self._createClear());
            }
            $self.element.append($column1);

            var $column2 = $('<div>').addClass('column');
            {
                $column2.append($self._createLabelElement('Release'));
                $column2.append($self._values.release);
                $column2.append($self._createClear());

                $column2.append($self._createLabelElement('Last Report'));
                $column2.append($self._values.lastReportUpdate);
                $column2.append($self._createClear());

                $column2.append($self._createLabelElement('Policy Updated'));
                $column2.append($self._values.lastPolicyUpdate);
                $column2.append($self._createClear());
            }
            $self.element.append($column2);

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
            $element.html(label);

            return $element;
        },

        _createValueElement: function() {
            var $element = $('<span>');

            $element.addClass('value');
            $element.html('&nbsp;');

            return $element;
        },

        _clearFields: function() {
            var $self = this;

            $self._values.ip.html('unknown');
            $self._values.osType.html('unknown');
            $self._values.flavour.html('unknown');
            $self._values.release.html('unknown');
            $self._values.lastReportUpdate.html('unknown');
            $self._values.lastPolicyUpdate.html('unknown');
        },

        _setHeader: function(hostname, colour) {
            var $self = this;

            $self._headerLabel.html(hostname);
            $self._colourIcon.removeClass('red green yellow blue black');
            $self._colourIcon.addClass(colour);
        },

        updateHostKey: function(hostKey) {
            var $self = this;

            var requestUrl = $self.options.baseUrl + '/host/info/' + hostKey;

            $.getJSON(requestUrl, function(host) {
                $self._setHeader(host.hostname, host.colour);

                $self._clearFields();
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