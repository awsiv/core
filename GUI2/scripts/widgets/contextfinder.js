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
            setContextClbkParam: ''
            
        },
        _init: function(){
            var self=this;
        },
        _create:function(){
            var self=this;
            self.dialoginit();
         
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
            self.ajaxloader=$('');
            self.repdialog=$('');
            self.dialogcontent = self.dialogContainer();
            self.dialogcontent.dialog({
                height: self.options.height,
                width: self.options.width,
                autoOpen: false,
                modal: true
            });

            self.dialogcontent.parent().addClass('customdlg contextfinder').removeClass('ui-widget-content');
            self.element.bind('click',function(event){
                event.preventDefault();
                self.dialogcontent.dialog('open')
            });
            self.repdialog.appendTo(self.dialogcontent).hide();
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
        loadpagebody:function(url){
            var self=this,
                submit_url=url;
            $.ajax({
                type: "POST",
                url: submit_url,
                data: self._context,
                dataType:"html",
                success: function(data) {

                    self.updateDataInContainer(data);
                    $(".contextfinder_wrapper").delegate('.class_selector',"click", function(event) {
                        event.preventDefault();
                        self.getInludeExclude();
                        self.bindClassfinder(this);
                    });

                    //add another condition field
                    $(".contextfinder_wrapper").delegate('.add_condition',"click", function(event) {
                        event.preventDefault();
                        var destination = $(this).attr('destination');
                        var fieldname   = $(this).attr('fieldname');
                        var $new_el = $(self._createNewElement(fieldname));
                        
                        $('td.' +destination).prepend($new_el);
                        $new_el.find('input').focus();
                        self.refreshTabindex();
                        self.setFocusFirstElement(destination);
                    });

                    //buttons inside input box
                    $(".contextfinder_wrapper").delegate('.delete_condition',"click", function(event) {
                        event.preventDefault();
                        $(this).parent().remove();
                        self.getInludeExclude();
                    });                    
    
                    //swap values
                    $(".contextfinder_wrapper").delegate(".invert","click",  function(event) {
                        event.preventDefault();    
     
                        // try to create copy of all elements..
                        var elem_includes   = $(self.dialogcontent.find('td.includes .item'));
                        var cloned_includes = elem_includes.clone(true);
                        
                        var elem_excludes   = $(self.dialogcontent.find('td.excludes .item'));
                        var cloned_excludes = elem_excludes.clone(true);
                        
                        //rename includes
                        $.each(cloned_includes, function() { 
                            cloned_includes.find('input').attr('name', 'exclude[]');
                        });
                        //rename excludes
                        $.each(cloned_excludes, function() { 
                            cloned_excludes.find('input').attr('name', 'include[]');                            
                        });
                        
                        // replace current includes/excludes with cloned 
                        if (self.dialogcontent.find('td.includes .item').length>0) {
                            self.dialogcontent.find('td.includes .item').replaceWith(cloned_excludes);
                        }
                        else {
                            self.dialogcontent.find('td.includes').append(cloned_excludes);
                        }
                        
                        if (self.dialogcontent.find('td.excludes .item').length > 0) {
                            self.dialogcontent.find('td.excludes .item').replaceWith(cloned_includes);
                        }
                        else {
                            self.dialogcontent.find('td.excludes').append(cloned_includes);
                        }
                      });

                    self.dialogcontent.find(".contextfinder_wrapper").delegate("#resetConditions","click",  function(event)  {
                        event.preventDefault();
                        self.resetForm();
                        self.refreshTabindex();
                    });

                    // bind enter(return)
                    self.dialogcontent.delegate(".contextfinder_wrapper", "keypress", function(event) {
                        if (event.keyCode == 13) {
                             self._dialogSubmit(event, self);
                        }
                    });
                    self.dialogcontent.find(".contextfinder_wrapper").delegate("#setConditions","click",  function(event)  {
                        self._dialogSubmit(event, self);
                    });
                    
                    
                    //add tooltips
                    var tooltip_classes = 'ui-tooltip-shadow ui-tooltip-dark';

                    self.dialogcontent.find('.add_condition').qtip({
                        content: {
                            text: 'Add condition'
                        },
                        style: {
                            classes: tooltip_classes
                        }
                    });  
                    
                    self.dialogcontent.find('.class_selector').qtip({
                        content: {
                            text: 'Select class'
                        },
                        style: {
                            classes: tooltip_classes
                        }
                    });  
                    
                    self.dialogcontent.find('.delete_condition').qtip({
                        content: {
                            text: 'Delete condition'
                        },
                        style: {
                            classes: tooltip_classes
                        }
                    });    
                    
                    self.dialogcontent.find('.invert').qtip({
                        content: {
                            text: 'Swap conditions'
                        },
                        style: {
                            classes: tooltip_classes
                        }
                    });      
                                        
                    
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
        refreshTabindex: function() {
            var self = this;
            var tabindex=1;
            self.dialogcontent.find('input[name="include[]"]').each(function(index)
            {
                $(this).attr('tabindex', tabindex);
                tabindex++;
                
            });
            self.dialogcontent.find('input[name="exclude[]"]').each(function(index)
            {
                $(this).attr('tabindex', tabindex);
                tabindex++;
            });
        },
        setFocusFirstElement: function(column) {
            var self= this;
            if(column == null)
            {
                self.dialogcontent.find('input').first().focus();
            }
            else 
            {
                self.dialogcontent.find('.'+ column + ' input').first().focus();
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
                }
            });
        },
        getInludeExclude: function () {
            var self = this;

            var includes = [];
            var excludes = [];

            self.dialogcontent.find('input[name="include[]"]').each(function(index)
            {
                if ($(this).val() != '')
                {
                    includes.push($(this).val());
                }
            });

            self.dialogcontent.find('input[name="exclude[]"]').each(function(index)
            {
                if ($(this).val() != '')
                {
                    excludes.push($(this).val());
                }
            });            
            
            // if no input elemenst  -reset context
            if (self.dialogcontent.find('input[name="includes[]"]').length == 0) {
                self._tmpcontext.includes = [];
            }
            if (self.dialogcontent.find('input[name="exclude[]"]').length == 0) {
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
              $('td.includes').append(self._createNewElement('include'));
              $('td.excludes').append(self._createNewElement('exclude'));
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
