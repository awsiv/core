(function($){
$.widget('ui.summaryreportfinder',
{
 options: {
            baseUrl:'',
            width:700,
            height:600,
            allhost:true,
            hostkey:"",
            loadUrl:'/summaryreports/search'
          },
_init: function(){

},
_create:function(){
        var self=this;
        self.dialoginit();
        self.loadpagebody();
        $.ui.reportfinder.instances.push(this.element);
},
dialoginit:function(){
  var self =this;
  self.ajaxloader=$('<div class="loading"></div>');
  self.repdialog=$('<div id="repdialog"></div>');
  self.dialogcontent = self.dialogContainer();
  self.dialogcontent.dialog({
                     height: self.options.height,
                     width: self.options.width,
                     autoOpen: false,
                     modal: true
   });
  self.dialogcontent.parent().addClass('customdlg').removeClass('ui-widget-content');
  self.element.bind('click',function(event){event.preventDefault();self.dialogcontent.dialog('open')});
  self.titlebar=self.dialogcontent.siblings('div.ui-dialog-titlebar');
  self.repdialog.appendTo(self.dialogcontent).hide();
  
},
loadpagebody:function(){
  var self=this;
  var fetchUrl = self.options.baseUrl + self.option.loadUrl;
  

},

dialogContainer: function() {
            var existing = $("#reportlistcontainer");
            if ( existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="reportlistcontainer" style="display:none" class="result" title="Reports"></div>').appendTo('body').
                dialog({
                    autoOpen: false
                });
                return requestDialog;
            }
        },

 destroy: function(){
			// remove this instance from $.ui.mywidget.instances
			var element = this.element,
				position = $.inArray(element, $.ui.reportfinder.instances);
			// if this instance was found, splice it off
			if(position > -1){
				$.ui.reportfinder.instances.splice(position, 1);
			}
			// call the original destroy method since we overwrote it
			$.Widget.prototype.destroy.call( this );
		}

});

$.extend($.ui.reportfinder, {
		instances: []
	});

})(jQuery);


