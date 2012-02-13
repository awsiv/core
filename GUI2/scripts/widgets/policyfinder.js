(function($){
    $.widget('ui.policyfinder',
    {
        options: {
            baseUrl: '',
            filterhandlerurl: "/widget/filterpolicy",
            width:700,
            height:600,
            defaultbehaviour:true,
            onlyShowHandle:false,
            showAddButton:false
        },
        dataLoaded : false,
        elementtext:"",
        page:2,
        selectedLetter:null,
        scrollingEnd:false,
        submitUrl:'',
        ajaxloader:$('<div class="loading"></div>'),
        originalTitle: '',
        
        _init: function(){
            var self=this;  
            self.resetPagination();
            $('#'+self.containerID()).bind('scroll',$.proxy(self.policylistscrolled,self));
        },
        
        _create:function(){
            var self=this;
            self.addsearchbar();
            self.addalphapager();
            $.ui.policyfinder.instances.push(this.element);
            $('#'+self.containerID()).delegate('a','click',$.proxy(self.handleSelected,self));
            $('#'+self.containerID()).delegate('.vblistadd','click',$.proxy(self.addSelected,self));
                      
        },
        
        resetPagination:function() {
            var self = this;
            self.page = 2;
            self.scrollingEnd = false;
            
        },
        
        containerID:function() {
            return this.element.attr('id') + '-'+'policyListContainer';
        },
        
        dialogContainer: function() {            
            var self = this;
            var existing = $('#' + self.containerID() );
            
            if ( existing.length > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="'+self.containerID()+'" style="display:none" class="result" title="Promises"><div id="policyList"><ul id="'+self.containerID()+'-ul"></ul></div></div>').appendTo('body').
                    dialog({
                    autoOpen: false,
                    title:'Promises',
                    beforeClose: function(event, ui) { 
                        // self.destroy();
                    }
                    
                }).bind('dialogclose', function(event, ui){
                    return false;
                });
                self.originalTitle =  requestDialog.dialog( "option", "title" );

                return requestDialog;
            }
        },
        
        policylistscrolled:function(event) {
            var $listpane=$(event.currentTarget);
            var self=this;
            if (self.scrollingEnd == true) return;
            
            
            if ($listpane[0].scrollHeight - $listpane.scrollTop() == $listpane.outerHeight()) {                   
                var url = self.submitUrl+'/'+self.page;               
                self.loadpagebody(url,"",true);   
                // $listpane.scrollTop($listpane.outerHeight());
                self.page++;              
            }
        },
        
        changeTitle:function(text) {
            var self = this;
            $('#'+self.containerID()).dialog('option', 'title', text);
        },
        
        revertTitle:function() {
            var self = this;
            $('#'+self.containerID()).dialog('option', 'title', self.originalTitle);
        },
        
        
        addsearchbar:function(){
            var self =this;
           
            self.dialogcontent = self.dialogContainer();
            self.dialogcontent.dialog($.extend({}, 
            $.ui.dialog.prototype.options, 
            self.options, 
            {
                autoOpen:false, 
                modal:true
            }
        ));
            self.dialogcontent.parent().addClass('customdlg').removeClass('ui-widget-content');
            self.titlebar=self.dialogcontent.siblings('div.ui-dialog-titlebar');
            //self.menuhandler=$('<span id="handle" class="operation">Options</span>');
            //self.titlebar.append(self.menuhandler).delegate('#handle','click',function(){self.menu.slideToggle();});

            
            if (!self.options.onlyShowHandle) {
                self.searchbar=$('<form id="policyfindersearch" action="'+self.options.baseUrl+'/widget/search_by_bundle"><span class="search"><input type="text" name="search" value="Search by bundle"/></span></form>')
            } else {
                self.searchbar=$('<form id="policyfindersearch" action="'+self.options.baseUrl+'/widget/search_by_handle"><span class="search"><input type="text" name="search" value="Search by handle"/></span></form>')
                
            }
            
            self.titlebar.append(self.searchbar).delegate('form','submit',$.proxy(self.searchpolicyfile,self));
            self.searchbar.delegate('input[type="text"]','click',function(){
                $(this).focus().select()
            });
            self.searchbar.delegate('input[type="text"]','focusin',$.proxy(self.searchboxevent,self));
            self.searchbar.delegate('input[type="text"]','focusout',$.proxy(self.searchboxevent,self));
            self.searchbar.find('input[type="text"]').data('default',self.searchbar.find('input[type="text"]').val());
            self.searchbar.delegate('input[type="text"]','keyup',$.proxy(self.searchbarkeyevent,self));

            self.menu=$('<div class="categories"><ul id="classoptions"></ul></div>');
            
            if (!self.options.onlyShowHandle) {
                self.menu.find('ul').append('<li>by bundle</li><li>by handle</li><li>by promiser</li><li>by type</li>');
            } else {
                self.menu.find('ul').append('<li>by handle</li>');
            }
             
            $('<span class="slider"></span>').appendTo(self.menu).bind('click',function(event){
                self.menu.slideUp();
            });
            self.menu.appendTo(self.titlebar).hide();
            self.menu.delegate('li','click',$.proxy(self.menuitemclicked,self));
            self.element.bind('click',function(event){
                event.preventDefault();
                self.elementtext=$(this).text();
                $(this).text('').append('<span class="loadinggif"> </span>');
                
                // check if it has already been opened before 
                if (!self.dataLoaded) {
                    self.submitUrl =  self.element.attr('href');
                    self.loadpagebody(self.submitUrl,"",true);
                    self.dialogcontent.dialog('open');
           
                } else {
                    // reshow the dialog
                    $('#' + self.containerID()).parent().show();
                }
                $(this).text(self.elementtext);
            });

        },

        menuitemclicked:function(event){
            var self=this;
            var sender=$(event.target);
            var selected_category=sender.text().toLowerCase();
            self.searchbar.find('input[type="text"]').val('Search '+sender.text().toLowerCase()).data('default','Search '+sender.text().toLowerCase())
            
            self.submitUrl = self.options.baseUrl+"/widget/search_"+selected_category.replace(/\s+/g, "_").toLowerCase();

            self.searchbar.attr("action", self.submitUrl);
            self.emptyContainer();
            
            self.loadpagebody( self.submitUrl ,"",false) ;
            self.searchbar.find('input[type="text"]').trigger('blur');
            self.alphasearch.find('li').removeClass('selected');
            sender.addClass('selected').siblings().removeClass('selected');
            //self.menu.fadeOut();
        },
        
        
        updateDataInContainer:function(data) {
            var self = this
              // To end the scrolling so that no further request is sent
            if (data.length == 0 || data ==null ) {
                self.scrollingEnd = true;
                self.revertTitle();
                return;
            }
            
            var containerUlId = self.containerID()+'-ul';
            document.getElementById(containerUlId).innerHTML += data;
            self.dataLoaded = true;            
            self.element.text(self. elementtext);
            self.revertTitle();                

        },
        
        emptyContainer: function() {
            var self = this;   
            var containerUlId = self.containerID()+'-ul';
            document.getElementById(containerUlId).innerHTML = '';
        },

        loadpagebody:function(url,val,escreg){
               
            var self=this,

            submit_url=url,
            searchval=val;
            var searchtext=self.searchbar.find('input[type="text"]').val();
            if(/search\s+by\s+/.test(searchtext)){
                searchtext='';
            };
            self.changeTitle('Loading...');     

            $.ajax({
                type: "POST",
                url: submit_url,
                data: {
                    filter:searchval,
                    reg:escreg,
                    type:searchtext,
                    showButton:self.options.showAddButton,
                    showOnlyHandle:self.options.onlyShowHandle                    
                },
                dataType:"html",
                success: function(data) {
                    self.updateDataInContainer(data);
                },
                error:function(jqXHR, textStatus, errorThrown){
                    var containerUlId = self.containerID()+'-ul';
                    //self.dialogcontent.html($("<ul>").attr("id", self.containerID()));
                    var li = '<li><span class="type">'+textStatus+'</span><p>'+errorThrown+'</p>';
                    document.getElementById(containerUlId).innerHTML = li ;
                    self.revertTitle();
                }
            });
           
           
        },
        
        handleSelected:function(event){
            var self=this,
            sender=$(event.target);
            if(!self.options.defaultbehaviour)
            {
                event.preventDefault();
                if(self.options.showAddButton){
                    self._trigger("handleClicked",event,{
                        selectedHandleName:sender.parent().parent().find('span.handle').html()
                    })   
                }else{
                    self._trigger("handleClicked",event,{
                        selectedHandleName:sender.html()
                    })
                }
            }            
        },
        
        addSelected:function(event){
            var self=this,
            sender=$(event.target);
            self._trigger("handleClicked",event,{
                selectedHandleName:sender.parent().find('span.handle').html()
            })  
        },
        
        searchboxevent:function(event)
        {
            var self=this;
            var searchbox=event.target
            if(searchbox.value==$(searchbox).data('default') && event.type=='focusin')
            {
                self.menu.slideDown();
                searchbox.value='';
            }
            if(searchbox.value=='' && event.type=='focusout')
            {
                searchbox.value=$(searchbox).data('default');
            }
        },
        searchpolicyfile:function(event)
        {
            event.preventDefault();
            var self=this,
            sender=$(event.target),
            searchval=sender.find('input').val();
            self.submitUrl=sender.attr('action'),
            self.emptyContainer();
            self.loadpagebody(self.submitUrl,searchval,true);
            self.menu.fadeOut();
        },

        addalphapager:function()
        {
            var self=this;
            var alphabets="A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z";
            var alphaarr=alphabets.split(',');
            self.alphasearch=$("<ul>").attr("class", "alphasearch");
            $.each(alphaarr, function(i,val) {
                var li = $("<li>");
                $("<a>").text(val).attr({
                    title:val, 
                    href:"#"
                }).appendTo(li);
                li.appendTo(self.alphasearch);
            });
            self.alphasearch.appendTo(self.dialogcontent.parent());
            self.alphasearch.delegate('a','click',$.proxy(self.sorton,self));
        },

        sorton:function(event){
            event.preventDefault();
            var self=this;
            var sender=$(event.target).parent();
            sender.addClass('selected').siblings().removeClass('selected');
            self.emptyContainer();            
            self.loadpagebody(self.searchbar.attr('action'),"^"+$(event.target).text(),false);
            if(self.menu.css('display')=='block')
            {
                self.menu.fadeOut(400);
            }
        },

        searchbarkeyevent:function(event){
            var self = this,
            searchbox= $(event.target),
            searchWord = searchbox.val();
            if(searchWord==''&& event.keyCode == 8)
            {
                self.menu.slideDown();
            }
        },

        destroy: function(){
            // remove this instance from $.ui.mywidget.instances
            var element = this.element,
            position = $.inArray(element, $.ui.classfinder.instances);
            // if this instance was found, splice it off
            if(position > -1){
                $.ui.classfinder.instances.splice(position, 1);
            }
            // call the original destroy method since we overwrote it
            $.Widget.prototype.destroy.call( this );
        },
        
        hideDialog:function() {
            this.dialogContainer().dialog('close');
        }


    });

    $.extend($.ui.policyfinder, {
        instances: []
    });

})(jQuery);


