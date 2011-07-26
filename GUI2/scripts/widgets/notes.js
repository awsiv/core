(function($) {
    var widgetNamespace = "uiExt";
    var widgetName = "ajaxyDialog";
    

    $.widget(widgetNamespace + "." + widgetName, {
        options: {
            extractTitleSelector: "h1, h2, h3, h4, h5",
            chainAjaxySelector: "a:not([target]), form:not([target])",
            closeDialogSelector: "a.dialog-close",
            width:'auto',
            originalElement: null, // store a refrence to the originally called element
            dontOverrideTitle : false,
            clickData:[]
            
        },
        
        _create: function() {
            var self = this;
            var element = self.element[0];
            
            if (element.tagName.toUpperCase() == "A") {
                $(element).bind("click."+self.widgetName,  function(event, ui) {
                    if (self.options.originalElement == null) {
                        self.options.originalElement = element;
                    }

                    self._handleClick();
                    return false;
                });
            }
            else if (element.tagName.toUpperCase() == "FORM") {
                $(element).bind("submit."+self.widgetName,  function(event, ui) {
                    if (self.options.originalElement == null) {
                        self.options.originalElement = element;
                    }

                    self._handleSubmit();
                    return false;
                });
            }
        },

        open: function() {
            var self = this;
            var element = self.element[0];

            if ( element.tagName.toUpperCase() == "A") {
                self._handleClick();
            } else if (element.tagName.toUpperCase() == "FORM") {
                self._handleSubmit();
            }
        },

        close: function() {
            this.dialogContainer().dialog("close");
        },

        _handleClick: function() {
            var self = this;
            var url = this.element.attr("href");
            var requestDialog = self.dialogContainer();
            self.titlebar=requestDialog.siblings('div.ui-dialog-titlebar');
            
            if (self.firstClicked != null)
                self.firstClicked = self.element;

            $("body").css("cursor", "progress");

            $.ajax({
                url: url,
                dataType: "html",
                data:self.options.clickData,
                type:"POST",
                success: function(resp, status, xhr) {
                    if (xhr.status != 0) {
                        self._loadToDialog(resp);
                    } else {
                        //stupid jquery calling this 'success', it's
                        //network unavailable.
                        self._displayFailure(url, xhr, status);
                    }
                },
                error: function(xhr, msg) {
                    self._displayFailure(url, xhr, msg);
                }
            });
        },

        _handleSubmit: function() {
            var self = this;
            var form = self.element;
            var actionUri = form.attr("action");
            var serialized = form.serialize();

            $("body").css("cursor", "progress");

            $.ajax({
                url: actionUri,
                data: serialized,
                type: form.attr("method").toUpperCase(),
                dataType: "html",
                success: function(resp, status, xhr) {
                    if (xhr.status != 0) {
                        var callback = self.options.change;
                        var nid = $(resp).find('input[name=nid]').val();
                        if ($.isFunction(callback) && nid) callback(nid,self.options.originalElement);
                        self._loadToDialog(resp);
                    } else {
                        //stupid jquery calling this 'success', it's
                        //network unavailable.
                        self._displayFailure(url, xhr, status);
                    }
                },
                error: function(xhr, msg) {
                    self._displayFailure(actionUri, xhr, msg);
                }
            });
        },

        _loadToDialog: function(html_content) {
            var self = this;
            var dialog = self.dialogContainer();
            //Cheesy way to restore it to it's default options, plus
            //our own local options, since its' a reuseable dialog.
            //for now we insist on modal:true.
            dialog.dialog($.extend({}, 
                $.ui.dialog.prototype.options, 
                self.options, 
                {
                    autoOpen:false, 
                    modal:true
                }
                ));
                    
             dialog.parent().removeClass('ui-widget-content').addClass('customdlg');
            if (self._trigger('beforeDisplay', 0, html_content) !== false) {
                dialog.html( html_content );

                //extract and set title
                if (!self.options.dontOverrideTitle) {
                var title;
                self.options.extractTitleSelector &&
                (title = dialog.find(self.options.extractTitleSelector).first().remove().text());
                title = title ||
                self.element.attr("title")
                title && dialog.dialog("option", "title", title);
                }
                //Make any hyperlinks or forms ajaxified, by applying
                //this very same plugin to em, and passing on our options.
                if (self.options.chainAjaxySelector) {
                    dialog.find(self.options.chainAjaxySelector).ajaxyDialog(self.options);
                }

                //Make any links marked dialog-close do so
                if ( self.options.closeDialogSelector ) {
                    dialog.find(self.options.closeDialogSelector).unbind("click." + widgetName);
                    dialog.find(self.options.closeDialogSelector).bind("click." + widgetName, function() {
                        dialog.dialog("close");
                        return false;
                    });
                }

                dialog.dialog("open");
            }
            $("body").css("cursor", "auto");
        },

        _displayFailure: function(uri, xhr, serverMsg) {
            if (  this._trigger("error", 0, {
                uri:uri,
                xhr: xhr,
                serverMsg: serverMsg
            }) !== false) {
                var dialog = this.dialogContainer();

                dialog.html("<div class='ui-state-error' style='padding: 1em;'><p><span style='float: left; margin-right: 0.3em;' class='ui-icon ui-icon-alert'></span>Sorry, a software error has occured.</p><p>" + uri + ": " + xhr.status + " "  + serverMsg+"</p></div>");
                dialog.dialog("option", "title", "Sorry, an error has occured.");
                dialog.dialog("option", "buttons", {
                    "OK": function() {
                        dialog.dialog("close");
                    }
                });
                dialog.dialog("open");
            }
            $("body").css("cursor", "auto");
        },

        // The DOM element which has a ui dialog() called on it.
        // Right now we insist upon modal dialogs, and re-use the same
        // <div>.dialog() for all of them. It's lazily created here.
        // If client calls dialog("destroy") on it, no problem, it'll
        // be lazily created if it's needed again.
        dialogContainer: function() {
            var existing = $("#reusableModalDialog");
            if ( existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="reusableModalDialog" style="display:none"></div>').appendTo('body').
                dialog({
                    autoOpen: false
                });
                return requestDialog;
            }
        }

    });
}(jQuery));