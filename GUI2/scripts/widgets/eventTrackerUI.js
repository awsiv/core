(function ($) {
    $.widget('ui.eventTrackerUI', {
        options: {
            contextWidgetElement:'hclist',
            baseUrl:''
        },
        _context: {
            includes: [],
            excludes: []
        },
        $busyIcon: $('<span class="loadinggif" style="display:none">').html('&nbsp;'),
        $errorDiv: $('<div>').addClass('error'),
       

        _create: function () {
            var $element = this.element;
            this.createAndListPane = $('<div>').attr('id','createList');
            $element.append(this.createAndListPane);
            this.loadTrackersAndCreateUI();
            this.bindEventForTrackerUI();
        },
        
        _modifyContext:function () {
            var $self = this;
            var includes = encodeURIComponent($self._context.includes);
            var excludes = encodeURIComponent($self._context.excludes);
            $self.filterPane.find('input[name=inclist]').val(includes);
            $self.filterPane.find('input[name=exlist]').val(excludes);
        },
        
        setContext: function(includes, excludes) {
            var $self = this;
            $self._context.includes = includes;
            $self._context.excludes = excludes;
            $self._modifyContext();
        },
        
        setHostContext: function(hostKey) {
            var $self = this;
            var includes = [common.canonify('PK_' + hostKey)];
            var excludes = [];
            $self.setContext(includes,excludes);
        },
        
      
        loadTrackersAndCreateUI: function() {
            var self = this;
            var viewUrl = self.options.baseUrl+'/eventTracker/createView';
            self.createAndListPane.load(viewUrl);
        },
        
        bindEventForTrackerUI:function(){
            var self=this;
            self.createAndListPane.delegate('form','submit',$.proxy(this.createButtonClicked,this))
            
        },
        
        createButtonClicked:function(event){
            event.preventDefault();
            var self=this,
                form=$(event.target),
                params = {
                 'data':form.serialize(),
                 'url':form.attr( 'action'),
                 'success': function (data) {
                     self.updateMessage(data);
                    },
                 'error':function(request, status, error){
                     self.updateMessage(request.responseText);
                 }
              };
            self.sendRequest(params);
        },
        
        sendRequest:function(params){
            var sendData=params.data,
                self=this;
            $.ajax({
                type: params.type ? params.type : 'POST' ,
                url: params.url,
                data: sendData,
                dataType: params.dataType ? params.dataType : 'json',
                success: function(data) {
                    if ($.isFunction(params.success)) {
                        return $.call(params.success(data));
                    }
                    return data;
                },
                error: function(jqXHR, textStatus, errorThrown) {
                    if ($.isFunction(params.error)) {
                        return $.call(params.error(jqXHR, textStatus, errorThrown));
                    }
                    self._displayFailure(jqXHR,textStatus, errorThrown);
                }
            });
        },
        
        updateMessage:function(message){
           var self=this;
           self.createAndListPane.find('#infoMessage').html(message);
        },
        
        _displayFailure: function(jqXHR,textStatus, errorThrown) {
            var serverMsg,
            self = this;
            if (jqXHR.status == 0) {
                serverMsg = 'You are offline!!\n Please Check Your Network.';
            }else if (jqXHR.status == 404) {
                serverMsg = 'Requested URL not found.';
            }else if (jqXHR.status == 500) {
                serverMsg = 'Internel Server Error. ' + jqXHR.responseText;
            }else if (errorThrown == 'parsererror') {
                serverMsg = 'Error.\nParsing JSON Request failed.';
            }else if (errorThrown == 'timeout') {
                serverMsg = 'Request Time out.';
            }else {
                serverMsg = 'Unknow Error.\n' + jqXHR.responseText;
            }
            self.updateMessage("<div class='ui-state-error' style='padding: 1em;width:90%'><p><span style='float: left; margin-right: 0.3em;' class='ui-icon ui-icon-alert'></span>" + ' ' + serverMsg + '</p></div>');
        },
      
        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        },
        
        openNewWindow: function(url, target) {
            window.open(url, target);
            return;
        }

    });

    $.extend($.ui.eventTrackerUI, {
        instances: []
    });
})(jQuery);
        


