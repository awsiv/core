(function($) {
    $.widget('ui.hostInfo', {
        options: {
            baseUrl: '',
            defaultbehaviour: true
        },
        header:$('<div class="header">'),
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

            $self.element.append($self.header);
            $self._colourIcon = $('<span>').addClass('colourIcon');
            $self.header.append($self._colourIcon);

            $self._headerLabel = $('<a>').addClass('headerLabel');
            $self.header.append($self._headerLabel);

            var $column1 = $('<div>').addClass('column');
                $column1.append($self._createDivWrapper($self._createLabelElement('IP: '),$self._values.ip));
                $column1.append($self._createDivWrapper($self._createLabelElement('OS Type: '),$self._values.osType));
                $column1.append($self._createDivWrapper($self._createLabelElement('Flavour: '),$self._values.flavour));


            var $column2 = $('<div>').addClass('column');
                $column2.append($self._createDivWrapper($self._createLabelElement('Release: '),$self._values.release));
                $column2.append($self._createDivWrapper($self._createLabelElement('Last Report: '),$self._values.lastReportUpdate));
                $column2.append($self._createDivWrapper($self._createLabelElement('Policy Updated: '),$self._values.lastPolicyUpdate));


            $self.element.append($column1).append($column2);

            $.ui.hostInfo.instances.push($self.element);
        },

        _createDivWrapper:function(){
            var $element = $('<div>');
             for (var i = 0, j = arguments.length; i < j; i++){
               $element.append(arguments[i]);
             }
            return $element;
        },

        _createLabelElement: function(label) {
            var $element = $('<span>');
            $element.html(label);
            return $element;
        },

        _createValueElement: function() {
            var $element = $('<span>');
            $element.html('&nbsp;');
            return $element;
        },

        _clearFields: function() {
            var $self = this;
            $self._headerLabel.html('');
            $self._values.ip.html('');
            $self._values.osType.html('');
            $self._values.flavour.html('');
            $self._values.release.html('');
            $self._values.lastReportUpdate.html('');
            $self._values.lastPolicyUpdate.html('');
        },

        _setHeader: function(hostkey, hostname, colour) {
            var $self = this;

            $self._headerLabel.html(hostname);
            $self._headerLabel.attr('href', $self._requestUrls.hostPage($self, hostkey));
            $self._colourIcon.removeClass('red green yellow blue black');
            $self._colourIcon.addClass(colour);
        },

        updateHostKey: function(hostKey) {
            var $self = this;

            var requestUrl = $self.options.baseUrl + '/host/info/' + hostKey;
            $self._clearFields();
            $self._ajaxLoader.show($self);
            $.getJSON(requestUrl, function(host) {
                $self._setHeader(hostKey, host.hostname, host.colour);
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
                    $self._values.lastReportUpdate.html(
                        common.time.format(common.unixTimeToJavascriptTime(host.lastReportUpdate)));
                }

                if (host.lastPolicyUpdate !== undefined) {
                    // NOTE: lastPolicyUpdate is in text format
                    $self._values.lastPolicyUpdate.html(host.lastPolicyUpdate + ' (agent timezone)');
                }
                $self._ajaxLoader.hide($self);
            });
        },

        _ajaxLoader:{
           show:function($self){
                $self._colourIcon.removeClass('colourIcon').addClass('loadinggif');
           },
           hide:function($self){
                $self._colourIcon.removeClass('loadinggif').addClass('colourIcon');
           }

        },

        _requestUrls: {

            hostPage: function(self, key) {
                return self.options.baseUrl + '/welcome/host/' + key;
            }
        },

        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }
    });

    $.extend($.ui.hostInfo, {
        instances: []
    });
})(jQuery);
