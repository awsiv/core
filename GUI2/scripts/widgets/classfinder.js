(function($) {
    var classfinder_animate = false;
    var initator=false;
    $.widget('ui.classfinder', 
    {     
        _context:{
            includes: [],
            excludes: []
        },

        options: {
            baseUrl: '',
            filterhandlerurl: '/widget/filterclass',
            defaultbehaviour: true,
            width: 700,
            height: 600,
             subscribe: ''
        },
        page:2,
        selectedLetter:null,
        scrollingEnd:false,
        elementtext:"",
        selectedMenu:null,
        animate:false,
        init:null,
        ajaxloader:$('<span class="loading2"></span>'),
        _init: function(){
            var self=this;  
            self.resetPagination();
            self.dialogcontent.bind('scroll', $.proxy(self.classlistscrolled, self));

            self.dialogcontent.ajaxError(function(e, jqxhr, settings, exception) {
                self._displayFailure(settings.url, jqxhr, exception);
            });
            self.setContext(self.options.includes,self.options.excludes);
            
            //fix to remember the link clicked
            self.element.bind('click',function(event){
                 initator=self.element;
                 self.init=self.element;
             });
            
        },
        _create:function(){
            var self=this;
             self.addsearchbar();
             self.addalphapager();
             
            self.element.bind('click',function(event){
                event.preventDefault();
                self. elementtext = $(this).text();
                $(this).text('').append('<span class="loadinggif"> </span>');

                self.dialogcontent.dialog('open');
                self.loadpagebody();

            });

            $.ui.classfinder.instances.push(this.element);
        },



        _displayFailure: function(uri,x,e) {
            var serverMsg,
            self = this;
            if (x.status == 0) {
                serverMsg = 'You are offline!!\n Please Check Your Network.';
            }else if (x.status == 404) {
                serverMsg = 'Requested URL not found.';
            }else if (x.status == 500) {
                serverMsg = 'Internel Server Error. ' + x.responseText;
            }else if (e == 'parsererror') {
                serverMsg = 'Error.\nParsing JSON Request failed.';
            }else if (e == 'timeout') {
                serverMsg = 'Request Time out.';
            }else {
                serverMsg = 'Unknow Error.\n' + x.responseText;
            }
            self.dialogcontent.html("<div class='ui-state-error' style='padding: 1em;width:90%'><p><span style='float: left; margin-right: 0.3em;' class='ui-icon ui-icon-alert'></span>Sorry, a software error has occured.</p><p>" + x.status + ' ' + serverMsg + '</p></div>');
            self.element.text(self.elementtext);
            self.revertTitle();
        },

        addsearchbar:function(){
            var self =this;
           
            self.dialogcontent = self.dialogContainer();
            self.dialogcontent.dialog({
                height: self.options.height,
                width: self.options.width,
                autoOpen: false,
                modal: true
            });
            self.dialogcontent.parent().addClass('customdlg').removeClass('ui-widget-content');
            self.titlebar = self.dialogcontent.siblings('div.ui-dialog-titlebar');
            self.filter = $('<ul>');
            self.filter.attr('id', 'cfinderfilter').addClass('classfilters').appendTo(self.dialogcontent.parent());

            self.gethostbtn = $('<span>').attr('id', 'findmatchedhost').text('view hosts');
            self.gethostbtn.appendTo(self.dialogcontent.parent()).hide();
            self.matchhostfinder = $('<form>').attr({
                id: 'finderform',
                action: self.options.baseUrl + '/search/index',
                method: 'post'
            }).hide();
            $('<input>').attr({
                id: 'name',
                name: 'name',
                type: 'hidden'
            }).appendTo(self.matchhostfinder);
            $('<input>').attr({
                name: 'hosts_only',
                type: 'hidden'
            }).val(true).appendTo(self.matchhostfinder);
            $('<input>').attr({
                name: 'host',
                type: 'hidden'
            }).val('All').appendTo(self.matchhostfinder);
            $('<input>').attr({
                name: 'report',
                type: 'hidden'
            }).val('contexts').appendTo(self.matchhostfinder);
            self.matchhostfinder.appendTo(self.dialogcontent.parent());
            //self.menuhandler=$('<span id="handle" class="operation">Options</span>');
            //self.titlebar.append(self.menuhandler).delegate('#handle','click',function(){self.menu.slideToggle();});
            
            self.searchbar=$('<form id="classfindersearch"><span class="search"><input type="text" name="search" value="Search by class (context)"/></span></form>')
            self.titlebar.append(self.ajaxloader);
            self.titlebar.append(self.searchbar).delegate('form', 'submit', {
                ui: self
            },function(event) {
                event.preventDefault();
            });
            self.searchbar.delegate('input[type="text"]', 'click', function() {
                $(this).focus().select();
            });
            self.searchbar.delegate('input[type="text"]','focusin',$.proxy(self.searchboxevent,self));
            self.searchbar.delegate('input[type="text"]','focusout',$.proxy(self.searchboxevent,self));
            self.searchbar.find('input[type="text"]').data('default',self.searchbar.find('input[type="text"]').val());
            self.searchbar.delegate('input[type="text"]','keyup',$.proxy(self.searchclassinlist,self));
            
            self.dialogcontent.parent().delegate('#findmatchedhost','click',$.proxy(self.findmatchedhost,self));
            self.menu=$('<div class="categories"><ul id="classoptions"></ul></div>');
            self.menu.find('ul').append('<li>All classes</li><li>Time classes</li><li>soft classes</li>');
            $('<span class="slider"></span>').appendTo(self.menu).bind('click', function(event) {
                self.menu.slideUp();
            });
            self.menu.appendTo(self.titlebar).hide();
            self.menu.delegate('li','click',$.proxy(self.menuitemclicked,self));
           
            
        },

             
        menuitemclicked:function(event){
            var self=this;
            var sender=$(event.target);
            self.selectedLetter = null; // reset the letter selection 
            self.resetPagination();
            self.resetScrollPosition();
            self.searchbar.find('input[type="text"]').val('Search on ' + sender.text().toLowerCase()).data('default', 'Search on ' + sender.text().toLowerCase());
            self.dialogcontent.html(self.ajaxloader);
            sender.addClass('selected').siblings().removeClass('selected');
            self.selectedMenu = sender.text().toLowerCase();

            var params = {
                'url': self.options.baseUrl + self.options.filterhandlerurl,
                'data': {
                    filter: sender.text().toLowerCase().split(' ')[0]
                },
                'success': function(data) {
                    self.loadDataInContainer(data);
                }
            };
            self.sendRequest(params);


            self.searchbar.find('input[type="text"]').trigger('blur');
            self.resetSelectedLetter();
        },
        
        classlistscrolled:function(event) {
            
            var listpane=event.currentTarget;
            var self=this;
             //console.log(self.scrollingEnd);
            if (self.scrollingEnd == true || $(listpane).scrollTop()==0 || classfinder_animate==true) return;
            // only do scrolling event when no menu option are selected or all classes is selected.
            if (self.selectedMenu == null) {
                self.selectedMenu = 'all classes';
            }
            var filter = self.selectedMenu.toLowerCase().split(' ')[0];

            if ($(listpane)[0].scrollHeight - ($(listpane).scrollTop()) <= ($(listpane).outerHeight() + 50)) {
                classfinder_animate = true;


                var url = self.options.baseUrl + self.options.filterhandlerurl + '/' + self.page;
                if (self.selectedLetter != null) {
                    url = url + '/' + self.selectedLetter;
                }
                self.changeTitle('Loading');


                var params = {
                    'url': url,
                    'data': {
                        filter: filter
                    },
                    'success': function(data) {
                        classfinder_animate = false;
                        self.loadDataInContainer(data, true);
                    }
                };
                self.sendRequest(params);

                self.page++;
            }

        },

        addclassfilter: function(event)
        {
            event.preventDefault();
            var self = this,
            sender = $(event.target),
            selectedclass = $(sender).attr('title');
            var li = $('<li>');
            li.text(selectedclass).data('filter', selectedclass).appendTo(self.filter);
            $('<a>').text('X').appendTo(li);
            self.filter.find('li').delegate('a', 'click', $.proxy(self.removeclassfilter, self));
            self.gethostbtn.show();
        },

        removeclassfilter: function(event)
        {
            event.preventDefault();
            var self = this;
            $(event.target).parent().remove();
            if (self.filter.find('li').length == 0)
            {
                self.gethostbtn.hide();
            }
        },
        findmatchedhost: function(event)
        {
            event.preventDefault();
            var self = this;
            var searchval = '';
            //var filters=new Array();
            self.filter.find('li').each(function(index) {
                searchval += $(this).data('filter') + '|';
            });
            searchval = searchval.substring(0, searchval.length - 1);
            self.matchhostfinder.find('#name').val(searchval);
            self.matchhostfinder.submit();
        },

        loadpagebody: function() {
            var self = this;
            self.changeTitle('Loading');
            var params = {
                'url': self.options.baseUrl + '/widget/allclasses',
                'success': function(data) {
                    self.loadDataInContainer(data, false);
                }
            };
            self.sendRequest(params);
        },

        loadDataInContainer: function(data,append) {
            var self = this;
            var previousListElement = document.getElementById('classList');
            var previousList = null;
            if (previousListElement != null && previousListElement.value != '') {
                previousList = previousListElement.innerHTML;
            }

            var list = [];
            var length = data.length;

            // To end the scrolling so that no further request is sent
            if (data.length == 0) {
                self.scrollingEnd = true;
            }

            for (var i = 0; i < length; i++) {
                list.push('<li>');
                var val = data[i];
                var viewHostLink = '';
                var addClassLink = '';
                var textLink = '<a class="name" title="' + val + '" href="' + self.options.baseUrl + '/search/index/host/All/report/contexts/name/'+ val + '">' + val + '</a>';
                if (self.options.defaultbehaviour) {
                    viewHostLink = '<a class="action btn" title="' + val + '" href="' + self.options.baseUrl + '/search/index/host/All/report/contexts/hosts_only/true/name/'+ val + '">' + 'View hosts' + '</a>';
                    addClassLink = '<a class="classadd btn" title="' + val + '">' + 'add to list' + '</a>';
                }
                list.push(textLink);
                list.push(viewHostLink);
                list.push(addClassLink);
                list.push('</li>');
            }

            var ul = '';
            if (append) {
                ul = '<ul id="classList">' + previousList + list.join('') + '</ul>';
            } else {
                ul = '<ul id="classList">' + list.join('') + '</ul>';
            }

            document.getElementById('classlistcontainer').innerHTML = ul;
            self.dialogcontent.find('#classList').delegate('a', 'click', $.proxy(self.classSelected, self));
            self.dialogcontent.find('#classList').delegate('a.classadd', 'click', $.proxy(self.addclassfilter, self));
            self.element.text(self.elementtext);
            self.revertTitle();
        },

        classSelected: function(event) {
            var self = this,
            sender = $(event.target);
            if (!self.options.defaultbehaviour)
            {
                event.preventDefault();
                self.dialogcontent.dialog('close')
                self._trigger("complete",null,{
                    selectedclass:sender.text(),
                    initator:initator
                });
            }
        },

        dialogContainer: function() {
            var self = this;
            var existing = $('#classlistcontainer');
            if (existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="classlistcontainer" style="display:none" class="result" title="Classes"><ul id="classList"></ul></div>').appendTo('body').
                dialog({
                    autoOpen: false,
                    beforeClose: function(event, ui) {
                        self.destroy();
                    }
                });
                self.originalTitle = requestDialog.dialog('option', 'title');
                return requestDialog;
            }

        },

        searchboxevent: function(event)
        {
            var self = this;
            var searchbox = event.target;
            if (searchbox.value == $(searchbox).data('default') && event.type == 'focusin')
            {
                self.menu.slideDown();
                searchbox.value = '';
            }
            if (searchbox.value == '' && event.type == 'focusout')
            {
                searchbox.value = $(searchbox).data('default');
            }
        },

        searchclassinlist: function(event)
        {
            var self=this,
                searchbox= $(event.target);
           // console.log(self.selectedMenu);
            // console.log(self.init);
            if (self.selectedMenu == null ) {
                self.selectedMenu='all classes';
            }
            var filter = self.selectedMenu.toLowerCase().split(' ')[0];

            var searchWord = encodeURIComponent(searchbox.val());

            if (event.keyCode == 13) {
                var url = self.options.baseUrl + self.options.filterhandlerurl + '/1/' + searchWord;
                self.selectedLetter = searchWord;
                var params = {
                    'url': url,
                    'data': {
                        filter: filter
                    },
                    'success': function (data) {
                        self.loadDataInContainer(data,false);
                    }
                };
                self.sendRequest(params);
            }
        },

        addalphapager: function()
        {
            var self = this;
            var alphabets = 'A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z';
            var alphaarr = alphabets.split(',');
            self.alphasearch = $('<ul>').attr('class', 'alphasearch');
            $.each(alphaarr, function(i,val) {
                var li = $('<li>');
                $('<a>').text(val).attr({
                    title: val,
                    href: '#'
                }).appendTo(li);
                li.appendTo(self.alphasearch);
            });
            self.alphasearch.appendTo(self.dialogcontent.parent());
            self.alphasearch.delegate('a', 'click', $.proxy(self.sorton, self));
        },

        sorton: function(event) {
            event.preventDefault();
            var self = this;
            var sender = $(event.target).parent();
            sender.addClass('selected').siblings().removeClass('selected');

            var clickedLetter = sender.text().toLowerCase();
            self.selectedLetter = '[' + clickedLetter + '|' + sender.text() + ']';
            if (self.selectedMenu == null) {
                self.selectedMenu = 'all classes';
            }

            self.resetPagination();
            self.resetScrollPosition();
            var url = self.options.baseUrl + self.options.filterhandlerurl + '/1/' + self.selectedLetter;
            var filter = self.selectedMenu.toLowerCase().split(' ')[0];
            self.changeTitle('Loading');

            var params = {
                'url': url,
                'data': {
                    filter: filter
                },
                'success': function (data) {
                    self.loadDataInContainer(data,false);
                }
            };
            self.sendRequest(params);

            if (self.menu.css('display') == 'block') {
                self.menu.fadeOut(400);
            }
        },

        resetPagination: function() {
            var self = this;
            self.page = 2;
            self.scrollingEnd = false;
        },

        resetSelectedLetter: function() {
            var self = this;
            self.selectedLetter = null,
            self.alphasearch.find('li').removeClass('selected');
        },

        resetScrollPosition: function() {
            var self = this;
            self.dialogcontent.scrollTop(0);
        },

        changeTitle: function(text) {
            var self = this;
            self.dialogcontent.dialog('option', 'title', text);
            self.ajaxloader.show();
        },

        revertTitle: function() {
            var self = this;
            self.dialogcontent.dialog('option', 'title', self.originalTitle);
            self.ajaxloader.hide();
        },

        destroy: function() {
            // remove the dialog list content before closing
            document.getElementById('classlistcontainer').innerHTML = '';
            var self = this;
            self.resetPagination();
            self.resetSelectedLetter();
            // remove this instance from $.ui.mywidget.instances
            var element = this.element,
            position = $.inArray(element, $.ui.classfinder.instances);
            // if this instance was found, splice it off
            if (position > -1) {
                $.ui.classfinder.instances.splice(position, 1);
            }
            // call the original destroy method since we overwrote it
            $.Widget.prototype.destroy.call(this);
        },
        beforeRequest: function(params) {
            var self = this;
        },

        setContext: function(includes, excludes) {
            var self = this;
            self._context.includes = includes;
            self._context.excludes = excludes;
        },
        sendRequest: function(params) {
            var self = this;

            if (self.options.subscribe) {
                var par = self.options.subscribe.getContext();
                self.setContext(par.includes, par.excludes);
            }

            var senddata = $.extend(params.data, self._context);
            $.ajax({
                type: params.type ? params.type : 'post' ,
                url: params.url,
                data: senddata,
                dataType: params.dataType ? params.dataType : 'json',
                success: function(data) {
                    if ($.isFunction(params.success)) {
                        return $.call(params.success(data));
                    }
                    return data;
                },
                error: function(data) {
                    if ($.isFunction(params.error)) {
                        return $.call(params.error());
                    }
                }
            });
        }

    });

    $.extend($.ui.classfinder, {
        instances: []
    });

})(jQuery);
