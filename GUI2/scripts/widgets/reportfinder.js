(function($){
$.widget('ui.reportfinder',
{
 options: {
            baseUrl:'',
            width:700,
            height:600,
            allhost:true,
            hostkey:""
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
  if(!self.options.allhost){
      self.repform=$('<form>').attr('action',self.options.baseUrl+'/search/').attr('method','post');
      self.repform.appendTo(self.dialogcontent);
  }
},
loadpagebody:function(){
  var self=this;
  self.reports =$("<ul>").attr("id", "reportList")
  $.getJSON(self.element.attr('href'), function(data) {
                             self.dialogcontent.append(self.reports);
                                  $.each(data,function(key, value) {
                                     var li = $("<li>").addClass('reportcategory').text(key);
                                     li.appendTo(self.reports);
                                     var ul=$("<ul>");
                                          $.each(value,function(i,val)
                                          {
                                            var li = $("<li>").addClass('reportitem');
                                            $("<a>").text(val['name']).data('id',val.id).attr({title:'Generates Report for: '+val['description'], href:"#"}).appendTo(li);
                                            $("<span class=\"repdesc\">").text(val['description']).appendTo(li);
                                            li.appendTo(ul); 
                                          });
                                       ul.appendTo(self.reports);  
                                  });
     });
 self.reports.delegate('a','click',$.proxy(self.reportselected,self))
 self.reports.delegate('li.reportcategory','click',$.proxy(self.categoryclicked,self));
},

categoryclicked:function(event){
 var sender=$(event.target);
   sender.next().toggle();
},

reportselected:function(event){
event.preventDefault()
var self=this,
    sender=$(event.target);
if(self.options.allhost)
    {
    self.repdialog.load(self.options.baseUrl+'/search/index', {report: sender.data('id')}, function() {
               self.repdialog.slideDown();
               var $closebtn=$("<a class='ui-dialog-titlebar-close ui-corner-all'><span class='ui-icon ui-icon-closethick'></span></a>");
                $(this).find('.panelhead').append($closebtn);
                $(this).find('input')[0].focus();
                 $closebtn.click(function(event){
                   event.preventDefault();
                   self.repdialog.fadeOut();
                 })

        });
    }
  else
      {
          self.repform.html('<input type="hidden" name="report" value="'+sender.text()+'" />');
          self.repform.append('<input type="hidden" name="host" value="'+self.options.hostkey+'" />');
          self.repform.submit();
      }
 
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


