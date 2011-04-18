(function($){
$.widget('ui.classfinder', 
{
 options: {
            filterhandlerurl: "/widget/filterclass",
            width:700,
            height:600

          },
_init: function(){
		
},
_create:function(){
        var self=this;
        self.addmenubtn();
        self.loadpagebody();
        $.ui.classfinder.instances.push(this.element);
},

addmenubtn:function(){
  var self =this;
  self.ajaxloader=$('<div class="loading"></div>');
  self.dialogcontent = self.dialogContainer();
  self.dialogcontent.dialog({
                     height: self.options.height,
                     width: self.options.width,
                     autoOpen: false,
                     modal: true
   });
  self.titlebar=self.dialogcontent.siblings('div.ui-dialog-titlebar');
  self.menuhandler=$('<span id="handle" class="operation">Options</span>');
  self.titlebar.append(self.menuhandler).delegate('#handle','click',function(){self.menu.slideToggle();});

        self.menu=$('<ul id="classoptions" class="btnoptions"></ul>');
        self.menu.append('<li>All classes</li><li>Time classes</li><li>Soft classes</li><li>Ip classes</li>');
        self.menu.appendTo(self.titlebar).hide();
        self.menu.delegate('li','click',$.proxy(self.menuitemclicked,self));
        self.element.bind('click',function(event){event.preventDefault();self.dialogcontent.dialog('open')});
},

menuitemclicked:function(event){
  var self=this;
  var sender=$(event.target);
  self.dialogcontent.html(self.ajaxloader);
           $.ajax({
                  type: "POST",
                  url: self.options.filterhandlerurl,
                  data: {filter:sender.text().toLowerCase().split(" ")[0]},
                  dataType:"json",
                  success: function(data) {
                  self.dialogcontent.html($("<ul>").attr("id", "classList")); //repeated can be merged with loadpagebody
                    $.each(data, function(i, val) {
                                        var li = $("<li>");
                                        $("<a>").text(val).attr({title:val, href:"#"}).appendTo(li);
                                        li.appendTo("#classList");
                                  });
                  }
           });
},

loadpagebody:function(){
  var self=this;
  $.getJSON(self.element.attr('href'), function(data) {
                             self.dialogcontent.html($("<ul>").attr("id", "classList"));
                               //$("<ul>").attr("id", "tagList").appendTo(self.classdlg);
                                  $.each(data, function(i, val) {
                                        var li = $("<li>");
                                        $("<a>").text(val).attr({title:val, href:"#"}).appendTo(li);
                                        li.appendTo("#classList");
                                  });
var alphabets="A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z";
var alphaarr=alphabets.split(',');
self.dialogcontent.alphasearch=$("<ul>").attr("id", "alphasearch");
$.each(alphaarr, function(i,val) {
	 var li = $("<li>");
         $("<a>").text(val).attr({title:val, href:"#"}).appendTo(li);
         li.appendTo(self.dialogcontent.alphasearch);
});
self.dialogcontent.alphasearch.appendTo(self.dialogcontent.parent());
     });
},

dialogContainer: function() {
            var existing = $("#classlistcontainer");
            if ( existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="classlistcontainer" style="display:none" class="result"></div>').appendTo('body').
                dialog({
                    autoOpen: false
                });
                return requestDialog;
            }
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

$.extend($.ui.classfinder, {
		instances: []
	});

})(jQuery);