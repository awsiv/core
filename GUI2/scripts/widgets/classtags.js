var tags  = {
    _create: function() {
    // alert('notes created')
    },
    options: {
        url: ""
    },

    _init: function() {
        var self = this;
        this.element.click(function(event){
            event.preventDefault();
            self.createtagcloud();
            return false;
        });
    },

    createtagcloud:function()
    {
      self = this;
        self.url = this.element.attr('href');
        // load the view and then save make a dialog out of it
        self.temp = $('<div style="display:hidden" title="Classes" id="tagCloud"></div>').appendTo('body');
        $.getJSON(self.options.url, function(data) {
               $("<ul>").attr("id", "tagList").appendTo(self.temp);
                  $.each(data, function(i, val) {
                        var li = $("<li>");
                        $("<a>").text(val[0]).attr({title:val[0], href:"#"}).appendTo(li);
                        li.appendTo("#tagList");
                     });
        self.temp.dialog({
                height: 'auto',
                width: 'auto',
                modal: true,
                close: function(event, ui) {
                    self.temp.remove();
                 }
                   });
                 });
         $(".ui-dialog-titlebar").hide();
         self.temp.delegate('a', 'click',self.addclassfilter);
    },

    addclassfilter:function(event)
    {
     var curclass= event.currentTarget.innerHTML
     self._trigger("complete",null,{selectedclass:curclass});
     self.temp.dialog('close');
    }
   
};
$.widget("ui.classtags", tags); // create the widget

