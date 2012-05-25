(function($) {
    $.widget('ui.hostfinder',
    {
         animate: false,
        _create: function() {
            var self = this;
            self.element.bind('click', function(event) {
                event.preventDefault();
                self.elementtext = $(this).text();
                $(this).text('').append('<span class="loadinggif"> </span>');
                self.createhostfinder();
                self.cfui.categories.hide();
                self.temp.dialog('open');
            });
            $.ui.hostfinder.instances.push(this.element);
        },
        cfui: {
            categories: '',
            resultpane: [],
            searchform: '',
            page: 2,
            selectedLetter: null,
            scrollingEnd: false,
            filtermethod: 'hostname',
            searchedkey: null,
            ajaxloader: $('<span class="loading2"></span>')
        },
        options: {
            baseUrl: '',
            url: '/widget/hostfinder',
            classhandler: '/widget/cfclasses',
            defaultbehaviour: true,
            height: 600,
            width: 735
        },
        elementtext: '',
        _init: function() {
            var self = this;
            self.options.url = self.options.baseUrl + self.options.url;
            self.cfui.scrollingEnd = false;
            self.animate=false;
        },
        createhostfinder: function()
        {
            var self = this;
            // load the view and then save make a dialog out of it
            self.temp = $('<div style="display:hidden" title="Find Host" id="hostfinderctrl"></div>').appendTo('body');
            $.ajaxSetup({
              error: function(jqxhr,exception) {self.displayFailure(jqxhr, exception);}
             });
            $.ajax({
                type: 'POST',
                url: self.options.url,
                data: {},
                dataType: 'html',
                async: false,
                success: function(data) {
                    self.temp.html(data);
                },
                error: function(jqxhr,exception) {self.displayFailure(jqxhr, exception);}
            });

                    self.cfui.categories = self.temp.find('#searchby');
                    self.cfui.resultpane = self.temp.find('#searchresult');
                    self.cfui.searchform = self.temp.find('#searchhost');
                    self.cfui.alphapaging = self.temp.find('#aplhaPaging');
                    self.$filter = $('#filters').find('ul');
                    self.temp.dialog({
                        height: self.options.height,
                        width: self.options.width,
                        autoOpen: false,
                        modal: true,
                        resizable: false,
                        close: function(event, ui) {
                            self.destroy();
                        }
                    });
             self.originalTitle = self.temp.dialog('option', 'title');
             self.element.text(self.elementtext);
             self.cfui.page = 2;

            //console.log( self.areas.categories);
            self.temp.parent().addClass('customdlg').removeClass('ui-widget-content');
            self.titlebar = self.temp.siblings('div.ui-dialog-titlebar');
            self.titlebar.append(self.cfui.ajaxloader);
            self.titlebar.append(self.cfui.searchform).delegate('form', 'submit', {
                ui: self
            },self.searchsubmit);

            self.cfui.categories.addClass('categories').appendTo(self.titlebar).hide();

            self.temp.siblings('div.ui-dialog-titlebar');
            self.cfui.categories.delegate('li', 'click', {
                ui: self
            },self.categoryselected);
            $('<span class="slider"></span>').appendTo(self.cfui.categories).bind('click', function(event) {
                self.cfui.categories.slideUp();
            });
            //self.temp.delegate('form','submit',{ui:self},self.searchsubmit);

            self.cfui.alphapaging.appendTo(self.temp.parent()).delegate('li', 'click', {
                ui: self
            },self.sortalphabetically);
            self.cfui.searchform.delegate('input[type="text"]', 'click', function() {
                $(this).focus().select();
            });
            self.cfui.searchform.delegate('input[type="text"]', 'focusin', {
                ui: self
            },self.searchboxevent);
            self.cfui.searchform.delegate('input[type="text"]', 'focusout', {
                ui: self
            },self.searchboxevent);
            
            self.cfui.searchform.delegate('input[type="text"]', 'keyup', {
                ui: self
            },self.searchboxkeyup);
            
             self.cfui.searchform.delegate('a.searchsubmit', 'click', {
                ui: self
            },function(){$(this).parent().parent('form').submit()});
            
            self.cfui.searchform.find('input[type="text"]').data('default', self.cfui.searchform.find('input[type="text"]').val());

            self.cfui.resultpane.delegate('a', 'click', $.proxy(self.hostselected, self));

            self.temp.bind('scroll', $.proxy(self.hostlistscrolled, self));
        },

       hostlistscrolled: function(event) {
            var listpane = event.currentTarget,
                 self = this,
                 url = self.options.url + '/' + self.cfui.page;
            
            //do nothing if the false scroll event triggred due to click event of alpha sort
            if ($(listpane).scrollTop() == 0 || self.cfui.scrollingEnd || self.animate == true) {
                return;
            }

            if (self.cfui.selectedLetter != null) {
                url = self.options.baseUrl + '/widget/sort_alphabetically_hostname/' + self.cfui.selectedLetter + '/' + self.cfui.page;
            }else if (self.cfui.searchedkey != null) {
                url = self.options.baseUrl + '/widget/search_by_' + self.cfui.filtermethod.replace(/\s+/g, '').toLowerCase() + '/' + encodeURIComponent(self.cfui.searchedkey) + '/' + self.cfui.page;
            }
            if ($(listpane)[0].scrollHeight - $(listpane).scrollTop() <= ($(listpane).outerHeight() + 400)) {
                   self.animate = true;
                   self.changeTitle('Loading');
                    $.get(url, function(data) {
                              if (data == '') {
                                  self.cfui.scrollingEnd = true;
                                  self.revertTitle();
                                  self.animate = false;
                                  return;
                              }


                              self.cfui.resultpane.find('ul').append(data);
                              self.cfui.page++;
                              self.animate = false;
                              self.revertTitle();
                          });
                  }
       },

      categoryselected: function(event)
        {
            var selected_category = $(this).text().toLowerCase();
            var self = event.data.ui;
            $(this).addClass('selected').siblings().removeClass('selected');
            self.cfui.filtermethod = selected_category;
            self.resetPagination();
            self.resetSelectedLetter();
            self.resetSearchKey();
            self.cfui.searchform.attr('action', self.options.baseUrl + '/widget/search_by_'+ selected_category.replace(/\s+/g, '').toLowerCase());
            self.cfui.searchform.find('input[type="text"]').val('search by ' + selected_category).data('default', 'search by ' + selected_category);
            if ($(this).attr('id') == 'search_class')
            {
                self.createclasstagcloud(self);
            }
            else {
                self.cfui.searchform.find('input[type="text"]').trigger('blur');
                self.cfui.searchform.find('input[type="text"]').trigger('focus');
            }
          //self.temp.unbind('scroll',$.proxy(self.hostlistscrolled,self));
        },

        hostselected: function(event)
        {
            var self = this,
            sender = $(event.target);
            if (!self.options.defaultbehaviour)
            {
                event.preventDefault();
                self._trigger('complete', event, {
                    selectedHostName: sender.html(),
                    selectedhost: sender.attr('title'),
                    report: self.options.report
                });
            }

        },

        searchsubmit: function(event) {
            event.preventDefault();
            var submit_url = $(this).attr('action');
            var searchval = $(this).find('input').val();
            var self = event.data.ui;
            self.cfui.categories.slideUp();
            self.resetPagination();
            self.resetSelectedLetter();
            self.cfui.searchedkey = searchval;

            if (self.cfui.filtermethod != 'class') {
                //self.cfui.resultpane.html(self.ajaxloader);
                  self.sendAjaxRequest(submit_url, {value: searchval});
                  self.resetScrollPosition();
            }
        },

        updatesearchresult: function(data) {
        //self.cfui.resultpane.html(data);
        },

        createclasstagcloud: function(ui) {
            var self = ui;
            self.classdlg = $('<div style="display:hidden" title="classes" id="tagCloud"></div>').appendTo('<body>');
            self.classdlg.focused = '';
            self.classdlg.dialog({
                height: self.options.height,
                width: 700,
                modal: true,
                position: 'center',
                close: function(event, ui) {
                    self.classdlg.remove();
                },
                open: function(event,ui) {
                    $(this).html(self.ajaxloader.clone().css({
                        'margin-top': $(this).height() / 2
                    }));

                    $.getJSON(self.options.baseUrl + self.options.classhandler, function(data) {
                        self.classdlg.html($('<ul>').attr('id', 'tagList'));
                        //$("<ul>").attr("id", "tagList").appendTo(self.classdlg);
                        $.each(data, function(i, val) {
                            var li = $('<li>');
                            $('<a>').text(val[0]).attr({
                                title: val[0],
                                href: '#'
                            }).appendTo(li);
                            li.children().css('fontSize', val[1] / 10 + 1 + 'em');
                            li.appendTo('#tagList');
                        });
                            var mylist = self.classdlg.find('#tagList');
                            var listitems = mylist.children('li').get();
                            listitems.sort(function(a, b) {
                                var compA = $(a).text().toUpperCase();
                                var compB = $(b).text().toUpperCase();
                                return (compA < compB) ? -1 : (compA > compB) ? 1 : 0;
                            });
                            $.each(listitems, function(idx, itm) {
                                mylist.append(itm);
                            });

                    });
                }
            });
            self.classdlg.titlebar = self.classdlg.siblings('div.ui-dialog-titlebar');
            self.classdlg.optionhandler = $('<span id="handle" class="operation">Options</span>');
            /*self.classdlg.titlebar.append(self.classdlg.optionhandler).delegate('#handle','click',function(){
                self.classdlg.options.slideToggle();
            });*/

            self.classdlg.options = $('<ul id="tgoptions"></ul>');
            self.classdlg.options.append('<li>Sort asc</li><li>Sort desc</li>');
            self.classdlg.options.appendTo(self.classdlg.titlebar).hide();

            self.classdlg.searchbar = $('<form id="searchclass"><span class="search"><input type="text" name="search"/></span></form>');
            self.classdlg.titlebar.append(self.classdlg.searchbar).delegate('form', 'submit', {
                ui: self
            },function(event) {
                event.preventDefault();
            });

            self.classdlg.matchedresult = $('<span id="tgmatchedresult">');
            self.classdlg.matchedresult.appendTo(self.classdlg.titlebar).hide();
            //search the tag cloud when user starts typing in the class
            self.classdlg.searchbar.delegate('input[type="text"]', 'click', function() {
                $(this).focus().select();
            });
            self.classdlg.searchbar.delegate('input[type="text"]', 'keyup', {
                ui: self
            },self.searchclassinlist);
            //delegate the options click event in classdialog title bar
            self.classdlg.options.delegate('li', 'click', {
                ui: self
            },self.actionontagcloud);
            //add a filter when a class is clicked in tag cloud
            self.classdlg.delegate('a', 'click', {
                ui: self
            },self.addclassfilter);
            //apply css in class tag item focys event and handle enter key events
            self.classdlg.delegate('li', 'focusin', {
                ui: self
            },self.tagfocusevent);
            self.classdlg.delegate('li', 'focusout', {
                ui: self
            },self.tagfocusevent);
            self.classdlg.delegate('li', 'keydown', {
                ui: self
            },self.tagcloudkeydown);
        },

        addclassfilter: function(event)
        {
            event.preventDefault();
            var self = event.data.ui;
            var selectedclass = $(this).text();
            //self._trigger("complete",null,{selectedclass:curclass});

            self.classdlg.dialog('close');
            var li = $('<li>');
            li.text(selectedclass).data('filter', selectedclass).appendTo(self.$filter);
            $('<a>').text('X').appendTo(li);
            var filters = new Array();
            self.$filter.find('li').each(function(index) {
                filters.push($(this).data('filter'));
            });
            self.cfui.resultpane.load(self.options.baseUrl + '/widget/ajaxlisthost/', {
                'filter': filters
            },function() {});
            self.$filter.find('li').delegate('a', 'click', {
                ui: self
            },self.removeclassfilter);
            self.cfui.categories.slideUp();
        },

        removeclassfilter: function(event)
        {
            event.preventDefault();
            var self = event.data.ui;
            $(this).parent().remove();
            var filters = new Array();
            self.$filter.find('li').each(function(index) {
                filters.push($(this).data('filter'));
            });
            self.cfui.resultpane.load(self.options.baseUrl + '/widget/ajaxlisthost/', {
                'filter': filters
            },function() {});
        },

        searchboxevent: function(event)
        {
            var self = event.data.ui;
            if (this.value == $(this).data('default') && event.type == 'focusin')
            {
                self.cfui.categories.slideDown();
            }
            if (this.value == '' && event.type == 'focusout')
            {
                this.value = $(this).data('default');
                self.cfui.categories.slideUp();
            }
        },

        sortalphabetically: function(event)
        {
            var self = event.data.ui,
                submit_url = self.options.baseUrl + '/widget/sort_alphabetically_hostname';
            self.cfui.selectedLetter = $(this).text();
            self.resetPagination();
            self.resetSearchKey();
            $(this).addClass('selected').siblings().removeClass('selected');
            self.cfui.resultpane.find('ul').empty();
            self.sendAjaxRequest(submit_url, {value: $(this).text()});
            self.resetScrollPosition();
            //self.cfui.categories.slideUp();
        },

        searchboxkeyup: function(event)
        {
            var self = event.data.ui;
            if (this.value == '' && event.keyCode == 8)
            {
                self.cfui.categories.slideDown();
            }
        },
        actionontagcloud: function(event)
        {

            var self = event.data.ui;
            if ($(this).text().toLowerCase() == 'sort asc')
            {
                var mylist = self.classdlg.find('#tagList');
                $(this).addClass('selected').siblings().removeClass('selected');
                var listitems = mylist.children('li').get();
                listitems.sort(function(a, b) {
                    var compA = $(a).text().toUpperCase();
                    var compB = $(b).text().toUpperCase();
                    return (compA < compB) ? -1 : (compA > compB) ? 1 : 0;
                });
                $.each(listitems, function(idx, itm) {
                    mylist.append(itm);
                });
            }


        },
        searchclassinlist: function(event)
        {
            //console.log($(this).val());
            var self = event.data.ui;
            var searchWord = $(this).val();
            if (searchWord.length >= 3) {
                var matched = 0;
                self.classdlg.find('#tagList').find('li').each(function() {
                    var text = $(this).text();
                    $(this).removeClass('selected');
                    if (text.match(RegExp(searchWord, 'i'))) {
                        $(this).addClass('selected');
                        matched++;
                    }
                });
                if (self.classdlg.matchedresult.css('display') == 'none')
                {
                    self.classdlg.matchedresult.text(matched + ' classes found').slideDown().delay(8000).fadeOut(400);
                }
            }
            else
            {
                self.classdlg.matchedresult.text(matched + ' classes found').slideUp();
            }

            //on backspace key pressed;
            if (this.value == '' && event.keyCode == 8)
            {
                self.classdlg.find('#tagList').find('li').each(function() {
                    $(this).removeClass('selected');
                });
            }
            //on enter key pressed
            if (event.keyCode == 13)
            {
                self.tagselectanothertag(self);
            }
        },
        tagselectanothertag: function(ui) {
            var self = ui;
            var total = self.classdlg.find('#tagList').find('li.selected').length;
            if (self.classdlg.focused == '' || self.classdlg.focused == total)
            {
                self.classdlg.focused = 0;
            }
            self.classdlg.find('#tagList').find('li.selected').eq(self.classdlg.focused).attr('tabindex', -1).focus();
        },
        tagcloudkeydown: function(event) {
            var self = event.data.ui;
            if (event.keyCode == 13)
            {
                self.tagselectanothertag(self);
            }
        },
        tagfocusevent: function(event) {
            if (event.type == 'focusin')
            {
                $(this).addClass('focused');
            }
            if (event.type == 'focusout')
            {
                $(this).removeClass('focused');
            }
        },
        resetPagination: function() {
            var self = this;
            self.cfui.page = 2;
            self.cfui.scrollingEnd = false;

        },

        resetSelectedLetter: function() {
            var self = this;
            self.cfui.selectedLetter = null,
            self.cfui.alphapaging.find('li').removeClass('selected');
        },

        resetSearchKey: function() {
            var self = this;
            self.cfui.searchedkey = null;
        },

       sendAjaxRequest: function(url,postdata) {
             var self = this;
             self.changeTitle('Loading');
             $.ajax({
                    type: 'POST',
                    url: url,
                    data: postdata,
                    dataType: 'html',
                    success: function(data) {
                       var ul = self.resultPane();
                        if (data == '') {
                          ul.html('<li>No host found</li>');
                        }else {
                           ul.html(data);
                        }
                        self.revertTitle();
                    }
                });
        },
        changeTitle: function(text) {
            var self = this;
             self.temp.dialog('option', 'title', text);
             self.cfui.ajaxloader.show();
        },

        revertTitle: function() {
            var self = this;
             self.temp.dialog('option', 'title', self.originalTitle);
             self.cfui.ajaxloader.hide();
        },

       resetScrollPosition: function() {
            var self = this;
            self.temp.scrollTop(0);
        },

        resultPane: function() {
            var self = this;
            var existing = self.cfui.resultpane.find('ul');
            if (existing.size() > 0) {
             return existing.first();
            }else {
                self.cfui.resultpane.html('<ul class="result"></ul>');
                return self.cfui.resultpane.find('ul');
            }
        },
        displayFailure: function(x,e) {
               var serverMsg,
                   self = this;
               if (x.status == 0) {
			serverMsg = 'You are offline!!\n Please Check Your Network.';
			}else if (x.status == 404) {
			serverMsg = 'Requested URL not found.';
			}else if (x.status == 500) {
			serverMsg = 'Internel Server Error.' + x.responseText;
			}else if (e == 'parsererror') {
			serverMsg = 'Error.\nParsing JSON Request failed.';
			}else if (e == 'timeout') {
			serverMsg = 'Request Time out.';
			}else {
			serverMsg = x.responseText;
			}

                if (self.cfui.resultpane.length > 0) {
                     self.cfui.resultpane.html("<div class='ui-state-error' style='padding: 1em;width:90%'><p><span style='float: left; margin-right: 0.3em;' class='ui-icon ui-icon-alert'></span>Sorry, a software error has occured.</p><p>" + x.status + ' ' + serverMsg + '</p></div>');
                }else {
                     self.temp.html("<div class='ui-state-error' style='padding: 1em;width:90%'><p><span style='float: left; margin-right: 0.3em;' class='ui-icon ui-icon-alert'></span>Sorry, a software error has occured.</p><p>" + x.status + ' ' + serverMsg + '</p></div>');
                }
                self.element.text(self.elementtext);
                self.revertTitle();
                self.animate = false;
        },

        destroy: function() {
            // remove this instance from $.ui.mywidget.instances
            var element = this.element;
            var self = this;
            self.resetPagination();
            self.resetSelectedLetter();
            self.cfui.resultpane = [];
            position = $.inArray(element, $.ui.classfinder.instances);
            // if this instance was found, splice it off
            if (position > -1) {
                $.ui.hostfinder.instances.splice(position, 1);
            }
            // call the original destroy method since we overwrote it
            $.Widget.prototype.destroy.call(this);
        },

        hideDialog: function() {
            this.temp.dialog('close');
        }


    });
    $.extend($.ui.hostfinder, {
        instances: []
    });

})(jQuery);
