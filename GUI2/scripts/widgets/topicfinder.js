(function($){
    $.widget('ui.topicfinder', 
    {
        options: {
            filterhandlerurl: "/widget/filtertopic",
            defaultbehaviour:true,
            width:600,
            height:580

        },
        _init: function(){
		
        },
        _create:function(){
    
            var self=this;
            self.addsearchbar();
            self.loadpagebody();
            self.addalphapager();
            $.ui.topicfinder.instances.push(this.element);
        
            self.element.bind('click',function(event){
                event.preventDefault();
                // load the content here 
                
                self.dialogcontent.dialog('open');
        
            }
            );

        },

        addsearchbar:function(){
            var self =this;
            self.ajaxloader=$('<div class="loading"></div>');
            self.dialogcontent = self.dialogContainer();
            self.dialogcontent.dialog({
                height: self.options.height,
                width: self.options.width,
                autoOpen: false,
                modal: true
            });
            self.dialogcontent.parent().addClass('customdlg').removeClass('ui-widget-content');
            self.titlebar=self.dialogcontent.siblings('div.ui-dialog-titlebar');
            //self.menuhandler=$('<span id="handle" class="operation">Options</span>');
            //self.titlebar.append(self.menuhandler).delegate('#handle','click',function(){self.menu.slideToggle();});

            self.searchbar=$('<form id="topicfindersearch"><span class="search"><input type="text" style="z-index:500000;" name="search" value="search on topic"/></span></form>')
            self.titlebar.append(self.searchbar).delegate('form','submit',{
                ui:self
            },function(event){
                event.preventDefault();
            });
            self.searchbar.delegate('input[type="text"]','click',function(){
                $(this).focus().select();
                
            });
            self.searchbar.delegate('input[type="text"]','focusin',$.proxy(self.searchboxevent,self));
            self.searchbar.delegate('input[type="text"]','focusout',$.proxy(self.searchboxevent,self));
            self.searchbar.find('input[type="text"]').data('default',self.searchbar.find('input[type="text"]').val());
            self.searchbar.delegate('input[type="text"]','keyup',$.proxy(self.searchclassinlist,self));
  
  
  
  
        },

        loadpagebody:function(){
            var self=this;
           
            $.get(self.element.attr('href'), function(data) {
                self.dialogcontent.html(data);
            //self.dialogcontent.find("#classList").delegate('a','click',$.proxy(self.classSelected,self));
            });
        },

        classSelected:function(event){
            var self=this,
            sender=$(event.target);
            if(!self.options.defaultbehaviour)
            {
                event.preventDefault();
                self._trigger("complete",null,{
                    selectedclass:sender.text()
                })
            }
            else{
                event.preventDefault();
            //    console.log(sender.text());
            }
        },

        dialogContainer: function() {
            var existing = $("#topiclistcontainer");
            if ( existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="topiclistcontainer" style="display:none" class="result" title="topics"></div>').appendTo('body').
                dialog({
                    autoOpen: false
                });
                return requestDialog;
            }
        },

        searchboxevent:function(event)
        {
            var self=this;
            var searchbox=event.target
            if(searchbox.value==$(searchbox).data('default') && event.type=='focusin')
            {
                searchbox.value='';
            }
            if(searchbox.value=='' && event.type=='focusout')
            {
                searchbox.value=$(searchbox).data('default');
            }
        },

        searchclassinlist:function(event)
        {
            var self=this;
            var searchbox= $(event.target);
            var searchWord = searchbox.val();

            code= (event.keyCode ? event.keyCode : event.which);
            // enter pressed
            if (code == 13 && searchWord.replace(/\s/g,"") != "") {
                var url = self.element.attr('href') ; //+ '/search/'+regex;
            
                $.post(url, {
                    search: searchWord
                },
                function(data) {
                    self.dialogcontent.html(data);
                });
           
            }

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
            self.alphasearch.delegate('li','click',$.proxy(self.sorton,self));
        },

        sorton:function(event){
            event.preventDefault();
            var self=this;
            var sender=$(event.target).parent();
            sender.addClass('selected').siblings().removeClass('selected');
            //console.log(RegExp("/^"+$(event.target).text()+"/", 'i'));
            var text = sender.text();
            var regex = '^['+text+'|'+text.toLowerCase()+']';
            var url = self.element.attr('href') ; //+ '/search/'+regex;
            
            $.post(url, {
                search: regex
            },
            function(data) {
                self.dialogcontent.html(data);
            });
            
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
        }

    });

    $.extend($.ui.topicfinder, {
        instances: []
    });

})(jQuery);