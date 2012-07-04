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
        timers:{},
        $busyIcon: $('<span class="loadinggif" style="display:none">').html('&nbsp;&nbsp;&nbsp;Loading'),
        $errorDiv: $('<div>').addClass('error'),
        eventDetails:{},
        eventOnSave:'',

        _create: function () {
            var $element = this.element;
            this.messagePane=$('<div>').attr('id','infoMessage');
            $element.append(this.messagePane);
            $element.append('<div class="grid_3"><a class="green_btn"><span id="createNewLink" class="green_btn">Create a tracker</span></a></div>');
            // this.createPane = $('<div>').attr('id','createPane').addClass('grid_5');
            this.createPane=this.loadTrackerCreateUI();
           
            this.listPane=$('<div>').attr('id','ListPane').addClass('grid_2').addClass('listPane');
            this.loadingPane=$('<div>').addClass('grid_3');
            this.$busyIcon.appendTo( this.loadingPane);
            $element.append(this.loadingPane);
            //$element.append(this.createPane);
            $element.append(this.listPane);
            $element.append('<div class="clear"></div>')
            this.loadTrackerCreateUI();
            this.refreshTrackersListUI();
            this.bindEventForTrackerUI();
        },
        
        contextModified:function(){
            var self=this;
            $.each(self.timers,function(index,val){
                self.eventDetails[index].inclist = encodeURIComponent(self._context.includes);
                self.eventDetails[index].exlist = encodeURIComponent(self._context.excludes);
                //only empty the containers
                self.resetEventViewerUI(index,true);
                //delete the from to request the data from the start time of tracker.
                if(self.eventDetails[index].hasOwnProperty('from')){
                    delete self.eventDetails[index]['from'];
                }
                
                var details={
                'id':index,
                'data':self.eventDetails[index],
                'success':function(){
                    self.hideLoading(self.loadingPane);
                },
                'error':function(jqXHR,textStatus, errorThrown){
                    self.hideLoading(self.loadingPane);
                    //self.resetTimers(index);
                    self._displayFailure(jqXHR,textStatus, errorThrown);
                  }
               };
                
                self.fetchEvents(details);
                self.loopTracker(details);
            });
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
                    startTimeString:tracker.attr('starttime'),
                    startTime:self.getTimeStamp(tracker.attr('starttime')),
                    report:tracker.attr('type'),
                    inclist:encodeURIComponent(self._context.includes),
                    exlist:encodeURIComponent(self._context.excludes),
                    operation:'edit'
                },
                'success':function(){
                    self.hideLoading(self.loadingPane);
                },
                'error':function(jqXHR,textStatus, errorThrown){
                    $(event.target).text('start');
                    self.hideLoading(self.loadingPane);
                    if(!details.data.hasOwnProperty('from')){
                        self.resetTimers(tracker.attr('id')); 
                    }
                    self._displayFailure(jqXHR,textStatus, errorThrown);
                }
            };
                     
       
            if(tracker.data('status') ==='stopped' || tracker.data('status')==undefined)
            {
                $(event.target).text('stop');
                self.displayLoading(self.loadingPane);
                tracker.data('status','started');
                self.fillformWithData(details.data); 
                //details.data.action='update';
                self.fetchEvents(details);
                self.loopTracker(details);
                self.refreshTrackerCreateUI();
            }
            else if(tracker.data('status') ==='started')
            {
                $(event.target).text('start');
                tracker.data('status','stopped');
                self.resetTimers(details.id);
                self.resetEventViewerUI(tracker.attr('id'));
            }
        },
        
        startButtonClicked:function(event){
           var self=this,
            btn=$(event.target),
            form=btn.parent().parent().parent().parent(),
            id=form.find('input[name=trackerName]').val(),
            $operation=form.find('input[name=oldName]'),
            $lastName=form.find('input[name=lastName]'),
            //eventId = id ==''? 'UnsavedTracker-'+Math.round((new Date()).getTime() / 1000) : id,
            unsavedTrackers=self.countUnSaved()+1,
            eventId = id ==''? 'UnsavedTracker-'+unsavedTrackers : id,
            details={
                'id' : eventId,
                'data':{
                    resource:form.find('input[name=resource]').val(),
                    startTimeString:form.find('input[name=time]').val(),
                    startTime:self.getTimeStamp(form.find('input[name=time]').val()),
                    report:form.find('select  option:selected').val(),
                    inclist:encodeURIComponent(self._context.includes),
                    exlist:encodeURIComponent(self._context.excludes)
                },
                'success':function(){
                    self.hideLoading(self.loadingPane);
                //self.updateMessage('<p class="info">Started Tracking</p>')
                },
                'error':function(jqXHR,textStatus, errorThrown){
                    btn.text('Start'); 
                    self.hideLoading(self.loadingPane);
                     if(!details.data.hasOwnProperty('from')){
                        self.resetTimers(eventId);
                      }
                    self._displayFailure(jqXHR,textStatus, errorThrown);
                }
            };
            
             if($lastName.length > 0 && $lastName.val() !=eventId){
                    self.handleIdEdit($lastName.val(),eventId);
             }
             
            if($operation.length > 0){
                details.data['operation']='edit';
                details.data['oldName']=$operation.val();     
            }
            
            self.displayLoading(self.loadingPane);
            self.fetchEvents(details);
            self.loopTracker(details);
            self.createPane.dialog('close');
        },
        
        formSubmitted:function(event){
            event.preventDefault();
            var self=this,
            form=$(event.target);
            var params =self.getFormParams(form);
            self.sendRequest(params);
        },
        
        createNewLinkClicked:function(event){
            var self=this;
            self.refreshTrackerCreateUI();
            self.showStartTrackerStep();
            self.createPane.dialog('open');
        },
        
        startHandlerClicked:function(event){
            event.preventDefault();
            var self=this,
            link=$(event.target);
            self.showStartTrackerStep();
        },
        
        deleteLinkClicked:function(event){
            var self=this,
            $tracker=$(event.target).parent(),
            params={
                'type':'GET',
                'url':self.options.baseUrl+'/eventTracker/delete/'+$tracker.attr('id'),
                'success':function(data){
                    if($tracker.data('status') =='started'){
                        self.resetTimers($tracker.attr('id'));
                        self.resetEventViewerUI($tracker.attr('id'));
                    }
                    $tracker.remove();
                }
            };
            self.sendRequest(params); 
        },
         
        editLinkClicked:function(event){
            var self=this,
            $tracker=$(event.target).parent(),
            data={
                'name':$tracker.attr('id'),
                'resource':$tracker.attr('resource'),
                'startTimeString':$tracker.attr('starttime'),
                'report':$tracker.attr('type'),
                'operation':'edit'
            };
            self.refreshTrackerCreateUI();
            self.fillformWithData(data); 
            self.showStartTrackerStep();
            self.createPane.dialog('open');
               
        },
         
        closeLinkClicked:function(event){
            var self=this;
            var eventID=$(event.target).parent().attr('eventid');
            self.resetTimers(eventID);
            self.resetEventViewerUI(eventID);  
            self.resetTrackers(eventID);
        },
         
        saveLinkClicked:function(event){
            var self=this;
            var eventID=$(event.target).parent().attr('eventid');
            var data=self.eventDetails[eventID];
            data.name=eventID;
            self.eventOnSave=eventID;
            //self.refreshTrackerCreateUI();
            console.log(data);
            self.fillformWithData(data);
            self.showSaveTrackerStep(eventID);
            self.createPane.dialog('open');
        },
         
        fillformWithData:function(data){
            var self=this,
            form=self.createPane.find('form');
            var $lastName=form.find('input[name=lastName]');
    
            form.find('input[name=resource]').val(data.resource);
            form.find('input[name=time]').val(data.startTimeString);
            form.find('input[name=trackerName]').val(data.name);
            form.find('select').val(data.report);
            if(data.hasOwnProperty('operation') && data.operation =='edit'){
                var $oldName=form.find('input[name=oldName]');
                
                var value=data.hasOwnProperty('oldName')?data.oldName:data.name;
                   
                if($oldName.length>0)
                {
                    $oldName.val(value);
                }
                else{
                    $('<input type="hidden">').attr('name','oldName').val(value).appendTo(form); 
                }
                form.find('input[type=submit]').val('Update');
                form.attr('action',self.options.baseUrl+'/eventTracker/update');   
            }
            
             if($lastName.length>0)
                {
                   $lastName.val(data.name);
                }
                else{
                    $('<input type="hidden">').attr('name','lastName').val(data.name).appendTo(form); 
                }
        },
         
        fetchEvents:function(details){
            var self=this,
            params={
                'data':details.data,
                'url':self.options.baseUrl+'/'+self.options.trackerUrl,
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
            details.data.from=parseInt((d.getTime() - interval)/1000);
            self.fetchEvents(details);
        },
        
        loopTracker:function(details){
            var self=this;
            var timer=setInterval(function(){
                $('#eventPane_'+details.id.hashCode()).find('div.eventLoader').show();
                self.fetchEventsFromNow(details)
            },self.options.interval);
            self.setEventTimers(details.id,timer);
            self.trackEventDetails(details.id,details.data);
        },
        
        setEventTimers:function(eventID,timer){
            var self=this;
            if( self.timers.hasOwnProperty(eventID)){
                clearInterval(self.timers[eventID]);
            }
            self.timers[eventID]=timer;
        },
        
        resetTimers:function(key){
            var self=this;
            if(key==undefined){
                $.each(self.timers,function(index,value){
                    clearInterval(value);
                    delete self.timers[index];
                    delete self.eventDetails[index];
                });  
            }
            else{
                
                clearInterval(self.timers[key]);
                delete self.timers[key];
                delete self.eventDetails[key];
               
            } 
        },
        
        trackEventDetails:function(id,details){
            var self=this;
            self.eventDetails[id]=details;  
        },
        
        countUnSaved:function(){
            var self=this,
                count=0;
            $.each(self.eventDetails,function(key,value){
                if(!value.hasOwnProperty('operation')){
                    count++;
                }
            });
            return count;
        },
        
        resetTrackers:function(id){
            var self=this;
            if(id==undefined){
                self.listPane.find('li').each(function(){
                    $(this).data('status','stopped').find('span.trackerStart').text('start');
                });
            }else{
                self.listPane.find('li').each(function(){
                    if($(this).attr('id')==id){
                        $(this).data('status','stopped').find('span.trackerStart').text('start');
                    }
                });
            }
        },
       
        loadTrackerCreateUI: function() {
            var self = this;
            var viewUrl = self.options.baseUrl+'/eventTracker/createView';
             
            var existing = $('#createPane');
            if (existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="createPane" style="display:none" title="Define tracker"></div>').load(viewUrl).appendTo('body').
                dialog({
                    autoOpen: false,
                    draggable: true,
                    modal: true,
                    resizable: false,
                    width: 415,
                    beforeClose: function(event, ui) {
                        //self.destroy();
                    }
                });
                self.originalTitle = requestDialog.dialog('option', 'title');
                return requestDialog;
            }
        },
        
        
        
        refreshTrackerCreateUI:function(){
            var self = this,
            form=self.createPane.find('form');
            form.find('input[type=submit]').val('Save');
            form.attr('action',self.options.baseUrl+'/eventTracker/create');
            form.find('input[type=hidden]').remove();
            self.createPane.find('input[type=text]').each(function(index){
                $(this).val('');
            });
            self.createPane.find('#testBtn').find('span').text('Start');
            self.showStartTrackerStep(); 
        },
        
        refreshTrackersListUI:function($tracker,id){
            var self = this,
            params={
                'url':self.options.baseUrl+'/eventTracker/listTrackers',
                'success':function(data){
                    if(data.length>0){
                        self.listPane.append('<span class="header"><em>Saved Trackers</em></span>')
                        var $list=$('<ul>');
                        $.each(data,function(index,value){
                            $list.prepend(self.createTracker(value));
                        });
                        self.listPane.find('span.header').append($list);
                    }
                }
            };
            
            if($tracker == undefined && id==undefined){
                self.sendRequest(params);  
            }
            else if(id==undefined){
                $list=self.listPane.find('ul')
                if($list.length==0){
                    self.listPane.append('<span class="header"><em>Saved Trackers</em><ul></ul></span>');
                }
                self.listPane.find('ul').append($tracker);    
            }
            else{
                self.listPane.find('li').each(function(){
                    if($(this).attr('id')==id){
                        $(this).remove();
                    }
                });
                self.listPane.find('ul').append($tracker);
            }    
        },
       
        handleStartedTimerToSavedTracker:function($tracker){
            var self=this,
            tempEventId=self.eventOnSave,
            eventDiv=self.element.find('#eventPane_'+tempEventId.hashCode()),
            newID=$tracker.attr('id'),
            details={
                'id':$tracker.attr('id'),
                'data':{
                    startTime:self.getTimeStamp($tracker.attr('startTime')),
                    startTimeString:$tracker.attr('startTime'),
                    resource:$tracker.attr('resource'),
                    report:$tracker.attr('type'),
                    inclist:encodeURIComponent(self._context.includes),
                    exlist:encodeURIComponent(self._context.excludes),
                    'operation':'edit'
                },
                'error':function(jqXHR,textStatus, errorThrown){
                    $tracker.find('span.trackerStart').text('start');
                    self.resetTimers($tracker.attr('id'));
                    self._displayFailure(jqXHR,textStatus, errorThrown);
                }
            };
         
            eventDiv.find('legend').text(newID);
            eventDiv.attr('id','eventPane_'+newID.hashCode());
            eventDiv.find('div.controls').attr('eventId',newID);
            self.resetTimers(tempEventId);
            self.loopTracker(details);
            $tracker.find('span.trackerStart').text('stop');
            $tracker.data('status','started');
          
        },
        
        handleIdEdit:function(oldID,newID){
           var self=this;
           var eventDiv=self.element.find('#eventPane_'+oldID.hashCode());
           eventDiv.find('legend').text(newID);
           eventDiv.attr('id','eventPane_'+newID.hashCode());
           eventDiv.find('div.controls').attr('eventId',newID);
           self.resetTimers(oldID);  
        },
        
        createTracker:function(value){
            var $tracker=$('<li>').attr(value);
            $('<span>').text(value.id).appendTo($tracker);
            $('<span>').text('start').addClass('trackerStart').appendTo($tracker);
            $('<span>').text('delete').addClass('trackerDelete').appendTo($tracker);
            $('<span>').text('edit').addClass('trackerEdit').appendTo($tracker);
            return $tracker;
        },
        
        showSaveTrackerStep:function(id){
            var self=this;
            if(id !=undefined){
                self.createPane.find('input[name=trackerName]').val(id);   
            }
       
            self.createPane.find('input[name=trackerName]').parent().show();
            self.createPane.find('#submitForm').show();
        },
        
        showStartTrackerStep:function(){
            var self=this;
            self.createPane.find('input[name=trackerName]').parent().hide();
            self.createPane.find('#submitForm').hide();
        },
        
        refreshEventViewerUI:function(id,data){
            var self=this;
            var eventViewer = self.element.find('#eventPane_'+id.hashCode());
            if(eventViewer.length == 0 && self.timers.hasOwnProperty(id)){
                var eventPane=$('<div>').attr('id','eventPane_'+id.hashCode()).addClass('eventPane');
                var controls=$('<div>').attr('eventId',id).addClass('controls').html('<span class="saveTracker editTracker btns" title="edit"></span><span class="closeTracker btns" title="close"></span>');
                controls.appendTo(eventPane);
                var container=$('<fieldset><legend>'+id+'</legend></fieldset>').appendTo(eventPane);
                var eventStatus=$('<div>').addClass('status');
                var eventList=$('<div>').addClass('events');
                var eventLoader=$('<div>').addClass('eventLoader').html('Loading...');
                container.append(eventStatus);
                container.append(eventList);
                self.element.append(eventPane);  
                eventViewer=self.element.find('#eventPane_'+id.hashCode());
                if( self.element.find('div.eventPane').length > 1){
                    eventViewer.addClass('largeMargin')
                }
                eventViewer.find('div.status').after(eventLoader);
            }
            eventViewer.find('div.eventLoader').hide();
            
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
                         
                    if(eventViewer.find('#'+eventId).length == 0){
                        eventViewer.find('div.events').prepend(event);
                    }
                })
            });
          
        },
        
        resetEventViewerUI:function(id,contextChanged){
            var self=this;
            if(id==undefined){
                $('div.eventPane').each(function(index){
                    $(this).remove();
                });
            }else{
                if(contextChanged!=undefined && contextChanged===true){
                    var $eventPane=$('#eventPane_'+id.hashCode());
                   $eventPane.find('div.eventLoader').show();
                   $eventPane.find('div.status').html('');
                   $eventPane.find('div.events').html('');
                }else{
                    $('#eventPane_'+id.hashCode()).remove();    
                }
            }
        },
        
        bindEventForTrackerUI:function(){
            var self=this;
            self.createPane.delegate('form','submit',$.proxy(this.formSubmitted,this));
            self.createPane.delegate('#testBtn','click',$.proxy(this.startButtonClicked,this));
            self.listPane.delegate('span.trackerStart','click',$.proxy(this.trackerClicked,this));
            self.createPane.delegate('#startViewhandle','click',$.proxy(this.startHandlerClicked,this));
            self.listPane.delegate('span.trackerDelete','click',$.proxy(this.deleteLinkClicked,this));
            self.listPane.delegate('span.trackerEdit','click',$.proxy(this.editLinkClicked,this));
            this.element.delegate('#createNewLink','click',$.proxy(this.createNewLinkClicked,this));
            this.element.delegate('span.saveTracker','click',$.proxy(this.saveLinkClicked,this));
            this.element.delegate('span.closeTracker','click',$.proxy(this.closeLinkClicked,this));
        },
        
      
        getFormParams:function(form){
            var self =this,
            params = {
                'data':form.serialize(),
                'url':form.attr('action'),
                'success': function (data) {
                    //self.updateMessage(data);
                    var $oldTracker=form.find('input[name=oldName]');
                    var $tracker=self.createTracker(data);
                    
                    if($oldTracker.length>0){
                        self.refreshTrackersListUI($tracker,$oldTracker.val()); 
                    }else{
                        self.refreshTrackersListUI($tracker); 
                    }
                    
                    self.handleStartedTimerToSavedTracker($tracker);
                    self.createPane.dialog('close');
                    
                },
                'error':function(request, status, error){
                    self.updateMessage(request.responseText);
                    self.createPane.dialog('close');
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
            element.find('.loadinggif').show();
        },
        
        hideLoading:function(element){
            element.find('.loadinggif').hide();
        },
        
        _displayFailure: function(jqXHR,textStatus, errorThrown) {
            var serverMsg,
            self = this;
            if (jqXHR.status == 0) {
                serverMsg = 'You are offline!!\n Please Check Your Network';
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
        
        setProperty:function(property,value){
            var self=this;
            $.each(self,function(key,val){
                if(!$.isFunction(key) && property === key){
                    property=val;
                }
            });
        },
        
        getProperty:function(property){
            var self=this;
            if(!$.isFunction(property)){
                return self[property]; 
            } 
            return undefined;
        },
      
        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }
    });

    $.extend($.ui.eventTrackerUI, {
        instances: []
    });
    
    String.prototype.hashCode = function(){
        var hash = 0;
        if (this.length == 0) return hash;
        for (var i = 0; i < this.length; i++) {
            var charc = this.charCodeAt(i);
            hash = ((hash<<5)-hash)+charc;
            hash = hash & hash; // Convert to 32bit integer
        }
        return hash;
    }


})(jQuery);
        


