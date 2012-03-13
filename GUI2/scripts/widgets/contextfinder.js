(function($){
    $.widget('ui.contextfinder',
    {
        _context:{
            includes: [],
            excludes: []
        },
        
        options: {
            baseUrl:'',
            width:590,
            height:300,
            allhost:true,
            autoOpen: false,
            resizable: false,
            hostkey:""
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
        
            self.element.bind('click',function(event){
                event.preventDefault();
                self.elementtext=$(this).text();
                $(this).text('').append('');
                self.dialogcontent.dialog('open');
                self.loadpagebody(self.element.attr('href'));
            });
        },

        setContext:function(includes,excludes) {
            var self = this;
            console.log(includes);
            self._context.includes = includes;
            self._context.excludes = excludes;          
            return;  
        },
        
        getContext:function() { // we use it when call contextfinder from clasfinder
            var self = this;
            self.getInludeExclude(); //call to get context if manually edited
            return self._context;
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
        _createNewElement: function (name) { //create new item
            var self=this;
           return $('<div class="item"><input type="text"  name="' + name + '[]" value=""><a class="class_selector" href="'+self.options.baseUrl+'/widget/allclasses">&nbsp;</a><a class="delete_condition" href="">&nbsp;</a><div class="clearboth"></div></div>');
        },
        loadpagebody:function(url){
            var self=this;
            
            self.getInludeExclude(); //call to get context if manually edited
            console.log( self._context);
            submit_url=url;
        
            $.ajax({
                type: "POST",
                url: submit_url,
                data: self._context,
                dataType:"html",
                success: function(data) {

                    self.updateDataInContainer(data);
            
                    self.bindClassfinder();
            

                    //clone items
                    $(".contextfinder_wrapper").delegate('.add_condition',"click", function(event) {
                        event.preventDefault();
                        var destination = $(this).attr('destination');
                        var new_el = self._createNewElement(destination);
                        $(this).before(new_el);
                        self.bindClassfinder();
                    });

                    $(".contextfinder_wrapper").delegate('.delete_condition',"click", function(event) {
                        event.preventDefault();
                        $(this).parent().remove();
                        self.getInludeExclude();
                    });                    
                    
                    $(".contextfinder_wrapper").delegate('.class_selector',"click",  function(event) {
                        event.preventDefault();
                        self.getInludeExclude();
                     });
      
        
                    //swap values
                    $(".contextfinder_wrapper").delegate(".invert","click",  function(event) {
                        event.preventDefault();    
                         
                        // try to create copy of all elements..
                        var elem_includes   = $('td.includes .item');
                        var cloned_includes = elem_includes.clone(true);
                        
                        var elem_excludes   = $('td.excludes .item');
                        var cloned_excludes = elem_excludes.clone(true);
                        
                        //rename includes
                        $.each(cloned_includes, function() { 
                            $(this).find('input').attr('name', 'exclude[]');
                        });
                        
                        //rename excludes
                        $.each(cloned_excludes, function() { 
                            $(this).find('input').attr('name', 'include[]');
                        });
                        
                        // replace current includes/excludes with cloned 
                        $('td.includes .item').replaceWith(cloned_excludes);
                        $('td.excludes .item').replaceWith(cloned_includes);

                      });

                    $(".contextfinder_wrapper").delegate("#send","click",  function(event)  {
                        event.preventDefault();
                        self.getInludeExclude();
                         self._trigger("complete",null,self.getInludeExclude());
                        self.dialogcontent.dialog('close')
                    });
    
                    self.animate=false;
                },
                error:function(jqXHR, textStatus, errorThrown){
                    var containerUlId = self.containerID();
                    var li = ''+textStatus+''+errorThrown+'';
                    document.getElementById(containerUlId).innerHTML = li ;
                }
            });
        },

        bindClassfinder: function () {
            var self= this;
            $('.class_selector').classfinder({
                defaultbehaviour:false,
                baseUrl:self.options.baseUrl,
                subscribe : this, // THIS instance of contextfinder, so we can call contextfinder functions from classfinder
                
                complete:function(event,data)
                {  
                    //$(this).siblings('input').val(data.selectedclass);
                    data.initator.siblings('input').val(data.selectedclass);
                    self.getInludeExclude();
                }
            });
        },
        getInludeExclude: function () {
            var self = this;

            self._context.includes = [];
            self._context.excludes = [];

 
            $('input[name="include[]"]').each(function(index)
            {
                if ($(this).val() != '')
                {
                    self._context.includes.push($(this).val());
                }
            });

            $('input[name="exclude[]"]').each(function(index)
            {
                if ($(this).val() != '')
                {
                    self._context.excludes.push($(this).val());
                }
            });            

            return self._context;
        },
        dialogContainer: function() {
            var existing = $("#contentfindercontainer");
            if ( existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="contentfindercontainer" style="display:none" class="result"></div>').appendTo('body').
                dialog({
                    autoOpen: false
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
            self.element.text(self.elementtext);
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
