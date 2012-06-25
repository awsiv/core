(function ($) {
    $.widget('ui.eventTrackerUI', {
        options: {
            contextWidgetElement:'hclist',
            baseUrl:'',
            trackerUrl:'eventTracker/track',
            interval:60000
        },
        _context: {
            includes: [],
            excludes: []
        },
        timers:[],
        $busyIcon: $('<span class="loadinggif" style="display:none">').html('&nbsp;'),
        $errorDiv: $('<div>').addClass('error'),
        tempEventDivs:[], //can be changed according to requirement
       

        _create: function () {
            var $element = this.element;
            this.messagePane=$('<div>').attr('id','infoMessage');
            $element.append(this.messagePane);
            this.createPane = $('<div>').attr('id','createPane').addClass('grid_5');
            this.listPane=$('<div>').attr('id','ListPane').addClass('grid_3').addClass('listPane');
            $element.append(this.createPane);
            $element.append(this.listPane);
            $element.append('<div class="clear"></div>')
            this.loadTrackerCreateUI();
            this.refreshTrackersListUI();
            this.bindEventForTrackerUI();
        },
        
        contextModified:function(){
            var self=this;
            self.resetTrackers();
            self.resetEventViewerUI();
        },
        
        setContext: function(includes, excludes) {
            var $self = this;
            $self._context.includes = includes;
            $self._context.excludes = excludes;
            $self.contextModified();
        },
        
        setHostContext: function(hostKey) {
            var $self = this;
            var includes = [common.canonify('PK_' + hostKey)];
            var excludes = [];
            $self.setContext(includes,excludes);
        },
        
        trackerClicked:function(event){
            var tracker=$(event.target).parent(),
            self=this,
            details={
               'id':tracker.attr('id'),
               'data':{
                   resource:tracker.attr('resource'),
                   startTime:tracker.attr('timestamp'),
                   report:tracker.attr('report'),
                   inclist:encodeURIComponent(self._context.includes),
                   exlist:encodeURIComponent(self._context.excludes)
                   },
               'success':function(){
                   self.hideLoading($(event.target));
                },
                'error':function(jqXHR,textStatus, errorThrown){
                  $(event.target).text('start');
                  self.hideLoading($(event.target));
                  self.resetTimers();
                  self._displayFailure(jqXHR,textStatus, errorThrown);
                }
            };
                     
           // self.resetTrackers(tracker.siblings('li'));
            if(tracker.data('status') ==='stopped' || tracker.data('status')==undefined)
            {
                $(event.target).text('stop');
                self.displayLoading($(event.target));
                tracker.data('status','started');
                self.fetchEvents(details);
                self.loopTracker(details);
            }
            else if(tracker.data('status') ==='started')
            {
                $(event.target).text('start');
                tracker.data('status','stopped');
                self.resetTimers();
                self.resetEventViewerUI('eventPane_'+tracker.attr('id'));
            }
        },
        
        startButtonClicked:function(event){
            var self=this,
            btn=$(event.target),
            divid = Math.round((new Date()).getTime() / 1000),
            form=btn.parent().parent().parent(),   
            details={
                'id' : divid,
               'data':{
                   resource:form.find('input[name=resource]').val(),
                   startTime:self.getTimeStamp(form.find('input[name=time]').val()),
                   report:form.find('select  option:selected').val(),
                   inclist:encodeURIComponent(self._context.includes),
                   exlist:encodeURIComponent(self._context.excludes)
                 },
                 'success':function(){
                    self.hideLoading(btn.parent());
                    self.showSaveTrackerStep();
                    self.tempEventDivs.push(divid);
                    //self.updateMessage('<p class="info">Started Tracking</p>')
                 },
               'error':function(jqXHR,textStatus, errorThrown){
                  btn.text('Start'); 
                  self.hideLoading(btn.parent());
                  self.resetTimers();
                  self._displayFailure(jqXHR,textStatus, errorThrown);
                }
            };
            //self.resetTrackers();
            if(btn.text().toLowerCase() == 'start')
            {
                btn.text('Stop');
                self.displayLoading(btn.parent());
                self.fetchEvents(details);
                self.loopTracker(details);
            }
            else if(btn.text().toLowerCase() == 'stop')
            {
                btn.text('Start');
                self.resetTimers();
                $.each(self.tempEventDivs,function(index,value){
                    self.resetEventViewerUI('eventPane_'+value);
                });
                self.tempEventDivs=[];
            }
        },
        
        createButtonClicked:function(event){
            event.preventDefault();
            var self=this,
            form=$(event.target),
            params =self.getFormParams(form);
            self.sendRequest(params);
        },
        
        startHandlerClicked:function(event){
            event.preventDefault();
            var self=this,
            link=$(event.target);
            self.showStartTrackerStep();
        },
        
        fetchEvents:function(details){
            var self=this,
            params={
                'data':details.data,
                'url':self.options.baseUrl+self.options.trackerUrl,
                'success':function(data){
                        if($.isFunction(details.success)){
                            $.call(details.success());
                          }
                     self.refreshEventViewerUI(details.id,data);
                     self.updateMessage('');
                 }
            };
             if($.isFunction(details.error)){
                 params['error']= details.error;
             }
            self.sendRequest(params);             
        },
        
        fetchEventsFromNow:function(details){
            var self=this,
            d = new Date(),
            interval=self.options.interval;
            details.data.startTime=parseInt((d.getTime() - interval)/1000);
            self.fetchEvents(details);
        },
        
        loopTracker:function(details){
            var self=this;
            var timer=setInterval(function(){
                $('#eventPane_'+details.id).find('div.status').html(self.$busyIcon.show());
                self.fetchEventsFromNow(details)
            },self.options.interval);
            self.timers.push(timer);
        },
        
        resetTimers:function(){
            var self=this;
            $.each(self.timers,function(index,value){
                clearInterval(value);
            });  
        },
        
        resetTrackers:function(trackers){
            var self=this;
            self.resetTimers();
            if(trackers==undefined){
                self.listPane.find('li').each(function(){
                    $(this).data('status','stopped').find('span.trackerStart').text('start');
                });
            }else{
                trackers.each(function(){
                    $(this).data('status','stopped').find('span.trackerStart').text('start');
                    $('#testBtn').find('span').text('Start'); //might need a clean up for input fields as well
                });
            }
        },
       
        loadTrackerCreateUI: function() {
            var self = this;
            var viewUrl = self.options.baseUrl+'/eventTracker/createView';
            self.createPane.load(viewUrl);
        },
        
        refreshTrackerCreateUI:function(){
            var self = this;
            self.createPane.find('input[type=text]').each(function(index){
                $(this).val('');
            });
        },
        
        refreshTrackersListUI:function(){
            var self = this,
            viewUrl = self.options.baseUrl+'/eventTracker/listTrackers';
            self.listPane.append(self.$busyIcon)
            self.listPane.load(viewUrl);
        },
        
        showSaveTrackerStep:function(){
            var self=this;
             self.createPane.find("#step1").hide();
             self.createPane.find("#step2").show();
        },
        
        showStartTrackerStep:function(){
             var self=this;
             self.createPane.find("#step1").show();
             self.createPane.find('#step2').hide();
        },
        
       refreshEventViewerUI:function(id,data){
           var self=this;
           var eventViewer = self.element.find('#eventPane_'+id);
           if(eventViewer.length == 0){
             var eventPane=$('<div>').attr('id','eventPane_'+id).addClass('eventPane'); 
             var eventStatus=$('<div>').addClass('status');
             var eventList=$('<div>').addClass('events');
             eventPane.append(eventStatus);
             eventPane.append(eventList);
             self.element.append(eventPane);  
              eventViewer=self.element.find('#eventPane_'+id);
           }
           
           var dt = new Date();
           var hostAndEventCount = 'No. of hosts seen: ' + data['meta']['hosts'] + ', No. of events: ' + data['meta']['events']+ ', Updated at: ' + dt.format("H:i:s d/m/y") ;
           eventViewer.find('div.status').html('<div class="eventType">Type: '+data['meta']['type']+'</div><div class="hostandEvents">'+hostAndEventCount+'</div><div class="clear"></div>');
            
           $.each(data['data'],function(timestamp,collections){
               $.each(collections,function(index,eventLog){
                   var eventId='event_'+eventLog[4];
                   var event=$('<div>').attr('id',eventId).addClass('event');
                   var dte = new Date(eventLog[3] * 1000);
                   event.append('<b>Date:</b> ' + dte.format("H:i:s d/m/y") + ' <b>Host:</b> ' + eventLog[0] + ' <b><br />Handle:</b> ' + eventLog[1]
			     + '<br /><b>Message:</b> ' + eventLog[2] );
                         
                   if($('#'+eventId).length == 0){
                       eventViewer.find('div.events').prepend(event);
                   }
               })
           });
          
        },
        
        resetEventViewerUI:function(id){
            if(id==undefined){
               $('div.eventPane').each(function(index){
               $(this).remove();
                });
            }else{
              $('#'+id).remove();  
            }
        },
        
        bindEventForTrackerUI:function(){
            var self=this;
            self.createPane.delegate('form','submit',$.proxy(this.createButtonClicked,this))
            self.createPane.delegate('#testBtn','click',$.proxy(this.startButtonClicked,this))
            self.listPane.delegate('span.trackerStart','click',$.proxy(this.trackerClicked,this))
            self.createPane.delegate('#startViewhandle','click',$.proxy(this.startHandlerClicked,this))
        },
        
        getFormParams:function(form){
            var self =this,
            params = {
                'data':form.serialize(),
                'url':form.attr( 'action'),
                'dataType':'html',
                'success': function (data) {
                    self.updateMessage(data);
                    self.refreshTrackersListUI();
                    self.refreshTrackerCreateUI();
                },
                'error':function(request, status, error){
                    self.updateMessage(request.responseText);
                }
            };
            return params;
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
            self.messagePane.html(message);
        },
        
        getTimeStamp:function(timeString){
            if(timeString==''){
                return 0;
            }
            var ts=timeString.split(' ');
            var YMD=ts[0].split('-');
            var HMS=ts[1].split(':');
            var myTS= new Date();
            myTS.setFullYear(YMD[0]);
            myTS.setMonth(YMD[1]-1);
            myTS.setDate(YMD[2]);
            myTS.setHours(HMS[0]);
            myTS.setMinutes(HMS[1]);
            var utcTimestamp=Date.UTC(YMD[0],YMD[1]-1,YMD[2],HMS[0],HMS[1],0,0)/1000;
            //return Math.round(myTS.getTime()/1000);
            return utcTimestamp
        },
        
        displayLoading:function(element){
            var self=this;
            element.hide();
            element.siblings('.loadinggif').show();
        },
        
        hideLoading:function(element){
           element.show(); 
           element.siblings('.loadinggif').hide();
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
            }else if (jqXHR.status == 400) {
                serverMsg = 'Bad Request \n'  + jqXHR.responseText;
            }else {
                serverMsg = 'Unknow Error.\n' + jqXHR.responseText;
            }
            self.updateMessage("<div class='ui-state-error' style='padding:1em'><p style='margin-bottom:0px'><span style='float: left; margin-right: 0.3em;' class='ui-icon ui-icon-alert'></span>" + ' ' + serverMsg + '</p></div>');
        },
      
        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }
    });

    $.extend($.ui.eventTrackerUI, {
        instances: []
    });
})(jQuery);
        


