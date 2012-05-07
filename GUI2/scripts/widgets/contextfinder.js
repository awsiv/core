(function($){
    $.widget('ui.contextfinder',
    {
        _context:{
            includes: [],
            excludes: []
        },
        // this is temporal context. We use it for store values for "seans", 
        // if user click on set context then _context = _tmpcontext, 
        // otherwise _context won't be changed
        _tmpcontext:{
            includes: [],
            excludes: []
        },        
        HTML_element : '', // html element for widget
        options: {
            title: '',
            baseUrl: '',
            width:590,
            height:300,
            allhost:true,
            autoOpen: false,
            resizable: false,
            hostkey:"",
            defaultEmptyElements: 2,
            
            setContextClbkFnc: '',
            setContextClbkParam: '',
            
            embedded: false,
            HTML_ID: '',
            doNotShowButtons: false,
            
            include_field_name: 'include',
            exclude_field_name: 'exclude'

            
        },
        _init: function(){
            var self=this;
        },
        _create:function(){
            var self=this;
            self.dialoginit();
            
        
                if (self.options.embedded == false) {
                    self.dialogcontent.dialog({
                        height: self.options.height,
                        width: self.options.width,
                        autoOpen: false,
                        modal: true
                    });
            
                    self.dialogcontent.dialog('option', 'title', self.options.title);   
            
                    self.element.bind('click',function(event){
                        event.preventDefault();
                        self.dialogcontent.dialog('open');
                        self.loadpagebody(self.element.attr('href'));
                        
                    });
                }
                else {
                    self.dialogcontent.dialog({});
                    self._setHTMLScope();
                    self._bindMouseEvents();
                    self._bindKeyboardEvents();
                    self._addTooltips();
                }
           
            mediator.installTo(self);
            self.subscribe('contextChange', function(data){
                      self.setContext(data.includes, data.excludes);
           });
        },

        setContext:function(includes,excludes) {
            var self = this;
            self._context.includes = includes;
            self._context.excludes = excludes;   
            return;  
        },
        
        getContext:function() { // we use it when call contextfinder from clasfinder
            var self = this;
            self.getInludeExclude(); //call to get context if manually edited
            return self._tmpcontext;
        },

        dialoginit:function(){
            var self =this;
            self.ajaxloader = $('');
            self.repdialog  = $('');
            self.dialogcontent = self.dialogContainer();
            
            if (self.options.embedded == false) {
                self.dialogcontent.dialog({
                    height: self.options.height,
                    width: self.options.width,
                    autoOpen: false,
                    modal: true
                });
                
                self.element.bind('click',function(event){
                    event.preventDefault();
                    self.dialogcontent.dialog('open')
                });
                self.repdialog.appendTo(self.dialogcontent).hide();
                
            }
           
            self.dialogcontent.parent().addClass('customdlg contextfinder').removeClass('ui-widget-content');

        },
        _dialogSubmit: function (event, dialogObj) {
            var self = dialogObj;            
     
            event.preventDefault();
            self.getInludeExclude();
            self._trigger("complete",null,self.getInludeExclude());
            self.setContext(self._tmpcontext.includes, self._tmpcontext.excludes)

            self.publish("contextChange",self._context);
            
            // call callback if set
            if (self.options.setContextClbkFnc != '' && $.isFunction(self.options.setContextClbkFnc)) {
                self.options.setContextClbkFnc.call(self, self.options.setContextClbkParam);
            }
            
            self.dialogcontent.dialog('close')
        },
        _createNewElement: function (name) { //create new item
           var self=this;
           return $('<div class="item"><input type="text"  name="' + name + '[]" value=""><a class="class_selector" href="'+self.options.baseUrl+'/widget/allclasses">&nbsp;</a><a class="delete_condition" href="">&nbsp;</a><div class="clearboth"></div></div>');
        },
        _getDataForPost:function() {
            var self = this;
            var data = {};
            
            data.include_field_name = self.options.include_field_name;
            data.includes =  self._context.includes;
            
            data.exclude_field_name = self.options.exclude_field_name;
            data.excludes =  self._context.excludes;     
            
            data.html_id = self.options.HTML_ID;
            
            data.doNotShowButtons = self.options.doNotShowButtons;
            data.embedded = self.options.embedded;
            
            return data;            
        },
        loadpagebody:function(url){
            var self=this,
                submit_url=url;
            $.ajax({
                type: "POST",
                url: submit_url,
                data: self._getDataForPost(),
                dataType:"html",
                success: function(data) {

                    self.updateDataInContainer(data);

                    self._setHTMLScope();

                    self._bindMouseEvents();
                    
                    self._bindKeyboardEvents();
                    self._addTooltips();
                    

                    self.setFocusFirstElement();
                    
                    self.animate=false;
                    
                    self.getInludeExclude();
                },
                error:function(jqXHR, textStatus, errorThrown){
                    var containerUlId = self.containerID();
                    var li = ''+textStatus+''+errorThrown+'';
                    document.getElementById(containerUlId).innerHTML = li ;
                }
            });
        },
        _setHTMLScope: function() {
            var self=this;  
            if (self.options.HTML_ID == '')
            {
                self.HTML_element = $(".contextfinder_wrapper");
            }
            else 
            {
                self.HTML_element  = $("#" + self.options.HTML_ID);
            }
        },
        
        _bindMouseEvents: function() {
             var self=this;
                    self.HTML_element.delegate('.class_selector',"click", function(event) {
                        event.preventDefault();
                        self.getInludeExclude();
                        self.bindClassfinder(this);
                       //self.bindBundleFinder(this);
                    });

                    //add another condition field
                    //$('#'+self.options.my_param).delegate('.add_condition',"click", function(event) {
self.HTML_element.delegate('.add_condition',"click", function(event) {
//self.dialogcontent.find('.contextfinder_wrapper').delegate('.add_condition',"click", function(event) {
                        event.preventDefault();
                        var destination = $(this).attr('destination');
                        var fieldname   = $(this).attr('fieldname');
                        var $new_el = $(self._createNewElement(fieldname));
                        
                        self.HTML_element.find('td.' +destination).prepend($new_el);
                        $new_el.find('input').focus();
                        self.refreshTabindex();
                        self.setFocusFirstElement(destination);
                    });

                    //buttons inside input box
                    self.HTML_element.delegate('.delete_condition',"click", function(event) {
                        event.preventDefault();
                        $(this).parent().remove();
                        self.getInludeExclude();
                    });                    
    
                    //swap values
                    self.HTML_element.delegate(".invert","click",  function(event) {
                        event.preventDefault();    
     
                        // try to create copy of all elements..
                        var elem_includes   = $(self.HTML_element.find('td.includes .item'));
                        var cloned_includes = elem_includes.clone(true);
                        
                        var elem_excludes   = $(self.HTML_element.find('td.excludes .item'));
                        var cloned_excludes = elem_excludes.clone(true);
                        
                        //rename includes
                        $.each(cloned_includes, function() { 
                            cloned_includes.find('input').attr('name', self.options.exclude_field_name + '[]');
                        });
                        //rename excludes
                        $.each(cloned_excludes, function() { 
                            cloned_excludes.find('input').attr('name', self.options.include_field_name + '[]');                            
                        });
                        
                        // replace current includes/excludes with cloned 
                        if (self.HTML_element.find('td.includes .item').length>0) {
                            self.HTML_element.find('td.includes .item').replaceWith(cloned_excludes);
                        }
                        else {
                            self.HTML_element.find('td.includes').append(cloned_excludes);
                        }
                        
                        if (self.HTML_element.find('td.excludes .item').length > 0) {
                            self.HTML_element.find('td.excludes .item').replaceWith(cloned_includes);
                        }
                        else {
                            self.HTML_element.find('td.excludes').append(cloned_includes);
                        }
                      });
                    
                    // this buttons could be disabled for some views
                    self.HTML_element.delegate("#resetConditions","click",  function(event)  {
                        event.preventDefault();
                        self.resetForm();
                        self.refreshTabindex();
                    });


                    self.HTML_element.delegate("#setConditions","click",  function(event)  {
                        self._dialogSubmit(event, self);
                    });
                      
        },
        _bindKeyboardEvents: function() {
            var self=this
            // bind enter(return)
            self.dialogcontent.delegate('.contextfinder_wrapper', "keypress", function(event) {
                if (event.keyCode == 13) {
                    self._dialogSubmit(event, self);
                }
            });
        },
        
        _addTooltips: function () {
             var self=this           
              //add tooltips
                    var tooltip_classes = 'ui-tooltip-shadow ui-tooltip-dark';

                    self.HTML_element.find('.add_condition').qtip({
                        content: {
                            text: 'Add condition'
                        },
                        style: {
                            classes: tooltip_classes
                        }
                    });  
                    
                    self.HTML_element.find('.class_selector').qtip({
                        content: {
                            text: 'Select class'
                        },
                        style: {
                            classes: tooltip_classes
                        }
                    });  
                    
                    self.HTML_element.find('.delete_condition').qtip({
                        content: {
                            text: 'Delete condition'
                        },
                        style: {
                            classes: tooltip_classes
                        }
                    });    
                    
                    self.HTML_element.find('.invert').qtip({
                        content: {
                            text: 'Swap conditions'
                        },
                        style: {
                            classes: tooltip_classes
                        }
                    });      
                                        
        },
        
        refreshTabindex: function() {
            var self = this;
            var tabindex=1;
            self.HTML_element.find('input[name="'+self.options.include_field_name+'[]"]').each(function(index)
            {
                $(this).attr('tabindex', tabindex);
                tabindex++;
                
            });
            self.HTML_element.find('input[name="'+self.options.exclude_field_name+'[]"]').each(function(index)
            {
                $(this).attr('tabindex', tabindex);
                tabindex++;
            });

            $('#setConditions').attr('tabindex', tabindex);
            tabindex++;

            
        },
        setFocusFirstElement: function(column) {
            var self= this;
            if(column == null)
            {
               self.HTML_element.find('input').first().focus();
            }
            else 
            {
                self.HTML_element.find('.'+ column + ' input').first().focus();
            }
        },
        bindClassfinder: function (elem) {
            var self= this;
            $(elem).classfinder({
                defaultbehaviour:false,
                baseUrl:self.options.baseUrl,
                subscribe : this, // THIS instance of contextfinder, so we can call contextfinder functions from classfinder
                autoopen:true,
                complete:function(event,data)
                {  
                    $(this).siblings('input').val(data.selectedclass);
                    self.getInludeExclude();
                    // set focus to the element
                    $(elem).parent().find('input').focus();
                    
                }
            });
        },
        bindBundleFinder: function(elem) {
            var self= this;
            $(elem).bundlefinder({
                defaultbehaviour:false,
                baseUrl:self.options.baseUrl,
                subscribe : this, // THIS instance of contextfinder, so we can call contextfinder functions from classfinder
                autoopen:true,
                complete:function(event,data)
                {  
                    $(this).siblings('input').val(data.selectedclass);
                    self.getInludeExclude();
                    // set focus to the element
                    $(elem).parent().find('input').focus();
                    
                }
            });
        },
        
        getInludeExclude: function () {
            var self = this;

            var includes = [];
            var excludes = [];

            self.dialogcontent.find('input[name="'+self.options.include_field_name+'[]"]').each(function(index)
            {
                if ($(this).val() != '')
                {
                    includes.push($(this).val());
                }
            });

            self.dialogcontent.find('input[name="'+self.options.exclude_field_name+'[]"]').each(function(index)
            {
                if ($(this).val() != '')
                {
                    excludes.push($(this).val());
                }
            });            
            
            // if no input elemenst  -reset context
            if (self.dialogcontent.find('input[name="'+self.options.include_field_name+'[]"]').length == 0) {
                self._tmpcontext.includes = [];
            }
            if (self.dialogcontent.find('input[name="'+self.options.exclude_field_name+'[]"]').length == 0) {
                self._tmpcontext.excludes = [];
            }

          
            self._tmpcontext.includes = includes;
            self._tmpcontext.excludes = excludes;               

            return self._tmpcontext;
        },        
        resetContext: function () {
            var self = this;
            self._tmpcontext.includes = [];
            self._tmpcontext.excludes = [];
        },
        resetForm: function() {
            var self = this;
            self.resetContext();
            $('td.includes').empty();
            $('td.excludes').empty();
            
            for (var i=0; i<self.options.defaultEmptyElements;i++) {
              $('td.includes').append(self._createNewElement(self.options.include_field_name));
              $('td.excludes').append(self._createNewElement(self.options.exclude_field_name));
            }
            self.setFocusFirstElement();
        },
        dialogContainer: function() {
            var self = this;
            var existing = $("#contentfindercontainer");
            if ( existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="contentfindercontainer" style="display:none" class="result"></div>').appendTo('body').
                dialog({
                    autoOpen: false,
                     beforeClose: function(event, ui) {
                    }
                });
                
              requestDialog.keypress(function(event) {
                if (event.keyCode == $.ui.keyCode.ENTER) {
                    //self.publish("contextChange",self._context);
                    $(this).dialog('close');
                    event.preventDefault();
                    return false;
                  }
                });
                return requestDialog;
            }
        },
        containerID:function() {
            return this.element.attr('id');
        },
        updateDataInContainer:function(data) {
            var self = this
            // To end the scrolling so that no further request is sent
            if (data.length == 0 || data ==null ) {
                self.scrollingEnd = true;
                return;
            }
        
            var containerUlId = self.containerID();
            document.getElementById('contentfindercontainer').innerHTML = data;
            
            self.dataLoaded = true; 
        },
   
        destroy: function(){
            // remove this instance from $.ui.mywidget.instances
            var element = this.element,
            position = $.inArray(element, $.ui.contextfinder.instances);
            // if this instance was found, splice it off
            if(position > -1){
                $.ui.contextfinder.instances.splice(position, 1);
            }
            // call the original destroy method since we overwrote it
            $.Widget.prototype.destroy.call( this );
        }
    });

    $.extend($.ui.contextfinder, {
        instances: []
    });

})(jQuery);
