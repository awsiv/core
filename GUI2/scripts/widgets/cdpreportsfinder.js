(function($){
$.widget('ui.cdpreportfinder',
{
options: {
            baseUrl:'',
            width:700,
            height:600
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
},
loadpagebody:function(){
  var self=this;
  self.reports =$("<ul>").attr("id", "cdpreportList")
  $.getJSON(self.element.attr('href'), function(data) {
                             self.dialogcontent.append(self.reports);
                                  $.each(data, function(i, val) {
                                        var li = $("<li>");
                                        $("<a>").text(val[0]).attr(
                                        {title:'Generates Report for: '+val[1],
                                         href:self.options.baseUrl+'/cdpreports/get_report/'+val[0]}).appendTo(li);
                                        $("<span class=\"repdesc\">").text(val[1]).appendTo(li);
                                        li.appendTo(self.reports);
                                  });
     });
// self.reports.delegate('a','click',$.proxy(self.reportselected,self))
},
reportselected:function(event){
event.preventDefault()
var self=this,
    sender=$(event.target);

},

dialogContainer: function() {
            var existing = $("#cdpreportlistcontainer");
            if ( existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="cdpreportlistcontainer" style="display:none" class="result" title="CDP Reports"></div>').appendTo('body').
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
$.extend($.ui.cdpreportfinder, {
		instances: []
 });
 
})(jQuery);


