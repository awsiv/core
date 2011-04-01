var popup  = {
    _create: function() {
       
    },
    _init: function() {
       this.initialize();
     },
    _refresh: function() {
        self.temp.load(this.options.url, {}, function(){
            self.temp.dialog({
                height: 'auto',
                width: 'auto',
                modal: true
            });          
        });
        return false;
    },

    initialize: function() {
        self = this;
      var title = this.options.title
        // load the view and then save make a dialog out of it 
        self.temp = $('<div style="display:hidden" title="'+ title + '"></div>').appendTo('body');
        self._refresh();
    }
};
$.widget("ui.popup", popup); // create the widget