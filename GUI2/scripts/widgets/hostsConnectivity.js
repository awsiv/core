(function($) {
    $.widget('ui.hostsConnectivity', {
        options: {
            baseUrl: '../',
            defaultbehaviour: true
        },
        ajaxloader:$('<span class="loadinggif"></span>'),
        header:$('<p class="title">Host Operations</p>'),
        _create: function() {
            var $self = this;
            
            $self.element.append($self.header);
            $self.ajaxloader.appendTo($self.header);
            $self.ajaxloader.hide();
            
            $self._blue = $self._createColourEntry('blue', 'hosts unreachable',
                'Hosts that have not been reached (their state is unknown)');
            $self.element.append($self._blue);

            $self._black = $self._createColourEntry('black', 'hosts with scheduling deviation',
                'Hosts failed to execute their agents for last 3 scheduled rounds.');
            $self.element.append($self._black);

            $self.element.append($self._createClear());
        },

        _init: function() {
            var $self = this;

            $.get($self._requestUrls.blueHostThreshold($self),
                function(blueHostThreshold) {
                    $self._blue.children('.colourEntryLabel').attr('title',
                        'Hosts that have not been reached ' +
                        '(their state is unknown) for more than: ' +
                        blueHostThreshold);
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
            $self.ajaxloader.show();
            $self.element.find('.colourEntryLabel').html('');
            $.getJSON($self._requestUrls.hostCountAll($self, $self._context.includes),
                function(data) {
                    $self._setHostCount($self._blue, data.blue, 'hosts unreachable');
                    $self._setHostCount($self._black, data.black, 'hosts with scheduling deviation');
                    $self.ajaxloader.hide();
                }).error(function(jqXHR,textStatus, errorThrown){
                    $self.ajaxloader.hide();
             });
            /*$.getJSON($self._requestUrls.hostCount($self, $self._context.includes, 'blue'),
                function(count) {
                    $self._setHostCount($self._blue, count, 'hosts unreachable');
                });
            $.getJSON($self._requestUrls.hostCount($self, $self._context.includes, 'black'),
                function(count) {
                    $self._setHostCount($self._black, count, 'hosts with scheduling deviation');
                });*/
        },

        _createClear: function() {
            var $element = $('<div>');

            $element.addClass('clear');

            return $element;
        },

        _createColourEntry: function(colour, label, tooltip) {
            var $self = this;

           var $entry = $('<div>');
            $entry.addClass('colourEntry');

            var $entryIcon = $('<span>');
            $entryIcon.addClass('colourEntryIcon '+colour).html('&nbsp;').appendTo($entry);

            var $entryLabel = $('<a>');
            $entryLabel.addClass('colourEntryLabel showqtip').attr('title', tooltip).data({my:'left center', at:'right center'})
            
            $entryLabel.click(function () {
                window.location.href = $self.options.baseUrl + '/welcome/hosts/' +
                    colour + '/' + encodeURIComponent($self._context.includes) + '/' +
                    encodeURIComponent($self._context.excludes);
            });
            $entry.append($entryLabel);

            $self._setHostCount($entry, 0, label);

            return $entry;
        },

        _setHostCount: function($entry, count, label) {
            var $entryLabel = $entry.children('.colourEntryLabel');

            $entryLabel.html(count + ' ' + label);
        },

        _context: {
            includes: [],
            excludes: []
        },

        _requestUrls: {

            blueHostThreshold: function(self) {

                return self.options.baseUrl + '/appsettings/bluehostthreshold';
            },
            
            hostCountAll:function(self,includes){
                 return self.options.baseUrl + '/host/countConnectivity?' +'includes=' + encodeURIComponent(includes);
            }
        }
    });

    $.extend($.ui.hostsCompliance, {
        instances: []
    });

})(jQuery);



