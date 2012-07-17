(function($) {
    $.widget('ui.policyfinder',
    {
        options: {
            baseUrl: '',
            filterhandlerurl: '/widget/filterpolicy',
            width: 700,
            height: 600,
            defaultbehaviour: true,
            onlyShowHandle: false,
            onlyShowBundle:false,            
            showAddButton: false,
            autoopen: false,
            default_policy_url: '/widget/allpolicies'
        },
        dataLoaded: false,
        elementtext: '',
        page: 2,
        selectedLetter: null,
        scrollingEnd: false,
        submitUrl: '',
        ajaxloader: $('<span class="loading2"></span>'),
        originalTitle: '',
        animate: false,
        tempId: '',
        dialogcontent :'',
        _init: function() {
            var self = this;
            self.resetPagination();

            if (self.options.onlyShowBundle != true) {
                $('#' + self.containerID()).bind('scroll', $.proxy(self.policylistscrolled, self));
            }
            

        },

        _create: function() {
            var self = this;
            
            var url = '';
            if (self.element.attr('href') != '' &&  self.element.attr('href') != undefined) {
                url  = self.element.attr('href');
            }
            else
            {
                url = self.options.baseUrl +  self.options.default_policy_url;
            }
                
           self.main_url = url;
           
            var date = new Date().getTime();
            self.tempId =  'tempId_' + date++
            self.addsearchbar();
            self.addalphapager();
            $.ui.policyfinder.instances.push(this.element);
            $('#' + self.containerID()).delegate('a', 'click', $.proxy(self.handleSelected, self));
            $('#' + self.containerID()).delegate('.vblistadd', 'click', $.proxy(self.addSelected, self));
            $('#' + self.containerID()).delegate('.only_bundle', 'click',    $.proxy(self.bundleSelected, self));

        },
        resetSearchBar: function() {
            var self = this;
            self.searchbar.find('input[type="text"]').val(self.searchbar.find('input[type="text"]').data('default'));
        },

        resetPagination: function() {
            var self = this;
            self.page = 2;
            self.selectedLetter = '';
            self.scrollingEnd = false;

        },

        containerID: function() {
            var self = this;
            var id = self.element.attr('id') ? self.element.attr('id') : self.tempId;
            return id + '-' + 'policyListContainer';
        },

        dialogContainer: function() {
            var self = this;
            var existing = $('#' + self.containerID());

            if (existing.length > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="' + self.containerID() + '" style="display:none" class="result" title="Promises"><div id="policyList"><ul id="' + self.containerID() + '-ul"></ul></div></div>').appendTo('body').
                    dialog({
                    autoOpen: self.options.autoopen,
                    title: 'Promises',
                    beforeClose: function(event, ui) {
                        // self.destroy();
                    }

                }).bind('dialogclose', function(event, ui) {
                    return false;
                });
                self.originalTitle = requestDialog.dialog('option', 'title');

                return requestDialog;
            }
        },

        policylistscrolled: function(event) {
            var $listpane = $(event.currentTarget);
            var self = this;
            if (self.scrollingEnd == true || self.animate == true) return;


            if ($listpane[0].scrollHeight - $listpane.scrollTop() <= ($listpane.outerHeight() + 400)) {
                self.animate = true;
                var url = self.submitUrl + '/' + self.page;
                self.loadpagebody(url, self.selectedLetter, true);
                self.page++;
            }
        },

        changeTitle: function(text) {
            var self = this;
            $('#' + self.containerID()).dialog('option', 'title', text);
            self.ajaxloader.show();
        },

        revertTitle: function() {
            var self = this;
            $('#' + self.containerID()).dialog('option', 'title', self.originalTitle);
             self.ajaxloader.hide();
        },


        addsearchbar: function() {
            var self = this;

            self.dialogcontent = self.dialogContainer();
            self.dialogcontent.dialog($.extend({},
            $.ui.dialog.prototype.options,
            self.options,
            {
                autoOpen: self.options.autoopen,
                modal: true
            }
        ));
            self.dialogcontent.parent().addClass('customdlg').removeClass('ui-widget-content');
            self.titlebar = self.dialogcontent.siblings('div.ui-dialog-titlebar');
            
            if (!self.options.onlyShowHandle && !self.options.onlyShowBundle) {
                self.searchbar = $('<form id="policyfindersearch" action="' + self.options.baseUrl + '/widget/search_by_bundle"><span class="search"><input type="text" name="search" value="Search by bundle"/></span></form>');
            }else if (self.options.onlyShowBundle == true) {
                self.searchbar = $('<form id="policyfindersearch" action="' + self.main_url +'"><span class="search"><input type="text" name="search" value="Search by bundle"/></span></form>');
            } else {
                self.searchbar = $('<span class="search"><form id="policyfindersearch" action="' + self.options.baseUrl + '/widget/search_by_handle"><input type="text" name="search" value="Search by handle"/><a href="#" class="searchsubmit" role="button">&nbsp;</a></form></span>');

            }

            self.titlebar.append(self.searchbar).delegate('form', 'submit', $.proxy(self.searchpolicyfile, self));
            self.titlebar.append(self.ajaxloader);
            self.searchbar.delegate('input[type="text"]', 'click', function() {
                $(this).focus().select();
            });
            self.searchbar.delegate('input[type="text"]', 'focusin', $.proxy(self.searchboxevent, self));
            self.searchbar.delegate('input[type="text"]', 'focusout', $.proxy(self.searchboxevent, self));
            self.searchbar.find('input[type="text"]').data('default', self.searchbar.find('input[type="text"]').val());
            self.searchbar.delegate('input[type="text"]', 'keyup', $.proxy(self.searchbarkeyevent, self));
            self.searchbar.delegate('a.searchsubmit', 'click', function(){$(this).parent('form').submit()});

            self.menu = $('<div class="categories"><ul id="classoptions"></ul></div>');

   
            if (!self.options.onlyShowHandle && !self.options.onlyShowBundle) {
                self.menu.find('ul').append('<li>by bundle</li><li>by handle</li><li>by promiser</li><li>by bundle type</li>');
            }
            else if (self.options.onlyShowBundle == true) {
                  self.menu.find('ul').append('<li>by bundle</li>');
                   
            } else {
                self.menu.find('ul').append('<li>by handle</li>');
            }
   

            $('<span class="slider"></span>').appendTo(self.menu).bind('click', function(event) {
                self.menu.slideUp();
            });
            self.menu.appendTo(self.titlebar).hide();
            self.menu.delegate('li', 'click', $.proxy(self.menuitemclicked, self));
            self.element.bind('click', function(event) {
                event.preventDefault();
                self.openFinder();
            });

        },
        
        
        openFinder: function () {
            var self = this;

            self.elementtext = self.element.text();
                self.element.text('').append('<span class="loadinggif"> </span>');

                // check if it has already been opened before
                if (!self.dataLoaded) {
                    

                    
                    self.submitUrl = self.main_url;
                    self.loadpagebody(self.submitUrl, '', true);
                    self.dialogcontent.dialog('open');

                } else {
                    // reshow the dialog
                    $('#' + self.containerID()).parent().show();
                }
                self.element.text(self.elementtext);
        },

        menuitemclicked: function(event) {
            var self = this;
            self.animate = true;
            var sender = $(event.target);
            var selected_category = sender.text().toLowerCase();
            self.searchbar.find('input[type="text"]').val('Search ' + sender.text().toLowerCase()).data('default', 'Search ' + sender.text().toLowerCase());

            self.submitUrl = self.options.baseUrl + '/widget/search_'+ selected_category.replace(/\s+/g, '_').toLowerCase();

            self.searchbar.attr('action', self.submitUrl);
            self.emptyContainer();
            self.resetPagination();
            self.resetSearchBar();
            self.loadpagebody(self.submitUrl, '', false);
            self.searchbar.find('input[type="text"]').trigger('blur');
            self.alphasearch.find('li').removeClass('selected');
            sender.addClass('selected').siblings().removeClass('selected');
            //self.menu.fadeOut();
        },


        updateDataInContainer: function(data) {
            var self = this;
            var containerUlId = self.containerID() + '-ul';
              // To end the scrolling so that no further request is sent
            if (data.length == 0 || data == null) {
                self.scrollingEnd = true;
                self.revertTitle();
                // append empty message if no data is returned and theres no previous data in container
                if (!document.getElementById(containerUlId).innerHTML) {
                    document.getElementById(containerUlId).innerHTML += '<li style="border:0"><div class="info">No data found</div></li>';
                }

                return;
            }

            document.getElementById(containerUlId).innerHTML += data;
            self.dataLoaded = true;
            self.element.text(self. elementtext);
            self.revertTitle();

        },

        emptyContainer: function() {
            var self = this;
            var containerUlId = self.containerID() + '-ul';
            document.getElementById(containerUlId).innerHTML = '';
        },


         _displayFailure: function(jqXHR,textStatus, errorThrown) {
            var serverMsg,
            self = this;
            if (jqXHR.status == 0) {
                serverMsg = 'You are offline!!\n Please Check Your Network.';
            }else if (jqXHR.status == 404) {
                serverMsg = 'Requested URL not found.';
            }else if (jqXHR.status == 500) {
                serverMsg = 'Internal Server Error. ' + jqXHR.responseText;
            }else if (errorThrown == 'parsererror') {
                serverMsg = 'Error.\nParsing JSON Request failed.';
            }else if (errorThrown == 'timeout') {
                serverMsg = 'Request Time out.';
            }else {
                serverMsg = 'Unknow Error.\n' + jqXHR.responseText;
            }
            self.dialogcontent.html("<div class='ui-state-error' style='padding: 1em;width:90%'><p><span style='float: left; margin-right: 0.3em;' class='ui-icon ui-icon-alert'></span>"  + ' ' + serverMsg + '</p></div>');
            self.element.text(self.elementtext);
            self.revertTitle();
        },

        loadpagebody: function(url,val,escreg) {

            var self = this,

            submit_url = url,
            searchval = val;
            var searchtext = self.searchbar.find('input[type="text"]').val();
            if (/search\s+by\s+/.test(searchtext)) {
                searchtext = '';
            }
            self.changeTitle('Loading');

            $.ajax({
                type: 'POST',
                url: submit_url,
                data: {
                    filter: searchval,
                    reg: escreg,
                    type: searchtext,
                    showButton: self.options.showAddButton,
                    showOnlyHandle: self.options.onlyShowHandle,
                    showOnlyBundle: self.options.onlyShowBundle                   
                },
                dataType: 'html',
                success: function(data) {
                    self.updateDataInContainer(data);
                    self.animate = false;
                },
                error: function(jqXHR, textStatus, errorThrown) {
                     self._displayFailure(jqXHR,textStatus, errorThrown);
                }
            });


        },

        handleSelected: function(event) {
            var self = this,
            sender = $(event.target);
            if (!self.options.defaultbehaviour)
            {
                event.preventDefault();
                if (self.options.showAddButton) {
                    self._trigger('handleClicked', event, {
                        selectedHandleName: sender.parent().parent().find('span.handle').html()
                    });
                }else {
                    self._trigger('handleClicked', event, {
                        selectedHandleName: sender.html()
                    });
                }
            }
        },

        addSelected: function(event) {
            var self = this,
            sender = $(event.target);
            self._trigger('handleClicked', event, {
                selectedHandleName: sender.parent().find('span.handle').html()
            });
        },

        bundleSelected: function(event) {
            var self = this,
            sender = $(event.target);
            var selectedBundle = sender.text();

                event.preventDefault();
                self.dialogcontent.dialog('close')
                self._trigger("complete", null,{                    
                    'selectedbundle': selectedBundle
                });
     
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
        searchpolicyfile: function(event)
        {
            event.preventDefault();
            var self = this,
            sender = $(event.target),
            searchval = sender.find('input').val();
            self.animate = true;
            self.submitUrl = sender.attr('action'),
            self.emptyContainer();
            self.resetPagination();
            self.selectedLetter = searchval;
            self.loadpagebody(self.submitUrl, searchval, true);
            self.alphasearch.find('li').removeClass('selected');
            self.menu.fadeOut();
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
            self.animate = true;
            self.emptyContainer();
            self.resetPagination();
            self.resetSearchBar();
            self.selectedLetter = '^['+ $(event.target).text() + '|' + $(event.target).text().toLowerCase() + ']';
            
            if (self.options.onlyShowBundle == false) {
                self.loadpagebody(self.searchbar.attr('action'), self.selectedLetter, false);
            }
            else  {
                self.loadpagebody(self.main_url, self.selectedLetter, false);
            }
            
            if (self.menu.css('display') == 'block')
            {
                self.menu.fadeOut(400);
            }
        },

        searchbarkeyevent: function(event) {
            var self = this,
            searchbox = $(event.target),
            searchWord = searchbox.val();
            if (searchWord == '' && event.keyCode == 8)
            {
                self.menu.slideDown();
            }
        },

        destroy: function() {
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

        hideDialog: function() {
            this.dialogContainer().dialog('close');
        }


    });

    $.extend($.ui.policyfinder, {
        instances: []
    });

})(jQuery);


