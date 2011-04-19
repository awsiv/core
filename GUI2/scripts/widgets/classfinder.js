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
        self.loadpagebody();
        self.addsearchbar();
        self.addalphapager();
        $.ui.classfinder.instances.push(this.element);
},

addsearchbar:function(){
  var self =this;
  self.ajaxloader=$('<div class="loading"></div>');
  self.dialogcontent = self.dialogContainer();
  self.dialogcontent.dialog({
                     height: self.options.height,
                     width: self.options.width,
                     autoOpen: false,
                     modal: true
   });
  self.dialogcontent.parent().addClass('customdlg').removeClass('ui-widget-content');
  self.titlebar=self.dialogcontent.siblings('div.ui-dialog-titlebar');
  //self.menuhandler=$('<span id="handle" class="operation">Options</span>');
  //self.titlebar.append(self.menuhandler).delegate('#handle','click',function(){self.menu.slideToggle();});

  self.searchbar=$('<form id="classfindersearch"><span class="search"><input type="text" name="search" value="search on all classes"/></span></form>')
  self.titlebar.append(self.searchbar).delegate('form','submit',{ui:self},function(event){event.preventDefault();});
  self.searchbar.delegate('input[type="text"]','click',function(){$(this).focus()});
  self.searchbar.delegate('input[type="text"]','focusin',$.proxy(self.searchboxevent,self));
  self.searchbar.delegate('input[type="text"]','focusout',$.proxy(self.searchboxevent,self));
  self.searchbar.find('input[type="text"]').data('default',self.searchbar.find('input[type="text"]').val());
  self.searchbar.delegate('input[type="text"]','keyup',$.proxy(self.searchclassinlist,self));

  
        self.menu=$('<ul id="classoptions" class="categories"></ul>');
        self.menu.append('<li>All classes</li><li>Time classes</li><li>Soft classes</li><li>Ip classes</li>');
        self.menu.appendTo(self.titlebar).hide();
        self.menu.delegate('li','click',$.proxy(self.menuitemclicked,self));
        self.element.bind('click',function(event){event.preventDefault();self.dialogcontent.dialog('open')});

},

menuitemclicked:function(event){
  var self=this;
  var sender=$(event.target);
  self.searchbar.find('input[type="text"]').val('search on '+sender.text().toLowerCase()).data('default','search on '+sender.text().toLowerCase())
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
  self.searchbar.find('input[type="text"]').trigger('blur');
  self.alphasearch.find('li').removeClass('selected');
  self.menu.fadeOut();
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
     });
},

dialogContainer: function() {
            var existing = $("#classlistcontainer");
            if ( existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="classlistcontainer" style="display:none" class="result" title="Classes"></div>').appendTo('body').
                dialog({
                    autoOpen: false
                });
                return requestDialog;
            }
        },

 searchboxevent:function(event)
    {
      var self=this;
      var searchbox=event.target
      if(searchbox.value==$(searchbox).data('default') && event.type=='focusin')
          {
              self.menu.slideDown();
              searchbox.value='';
          }
      if(searchbox.value=='' && event.type=='focusout')
          {
              searchbox.value=$(searchbox).data('default');
          }
    },

   searchclassinlist:function(event)
    {
        var self=this;
        var searchbox= $(event.target);
        var searchWord = searchbox.val();
            if (searchWord.length >= 3) {
                //console.log(self.dialogcontent.find("#classList").find('li').length)
                if(self.menu.css('display')=='block')
                  {
                  self.menu.fadeOut(400);
                  }
                self.dialogcontent.find("#classList").find('li').each(function() {
                    var text = $(this).text();
                    $(this).css('display','none');
                       
                    if (text.match(RegExp(searchWord, 'i'))) {
                        $(this).css('display','block');
                    }
              });
         }
          if(searchWord==''&& event.keyCode == 8)
          {
               self.dialogcontent.find("#classList").find('li').each(function() {
                        $(this).css('display','block');
              });
              self.menu.slideDown();
          }
    },

  addalphapager:function()
  {
     var self=this;
     var alphabets="A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z";
     var alphaarr=alphabets.split(',');
     self.alphasearch=$("<ul>").attr("class", "alphasearch");
     $.each(alphaarr, function(i,val) {
	 var li = $("<li>");
         $("<a>").text(val).attr({title:val, href:"#"}).appendTo(li);
         li.appendTo(self.alphasearch);
    });
    self.alphasearch.appendTo(self.dialogcontent.parent());
    self.alphasearch.delegate('li','click',$.proxy(self.sorton,self));
  },

  sorton:function(event){
   event.preventDefault();
   var self=this;
   var sender=$(event.target).parent();
   sender.addClass('selected').siblings().removeClass('selected');
   //console.log(RegExp("/^"+$(event.target).text()+"/", 'i'));
   self.dialogcontent.find("#classList").find('li').addClass('unmatched').each(function() {
                    var text = $(this).text();
                    if (text.match(RegExp("^"+sender.text().toLowerCase(), 'i'))) {
                        $(this).removeClass('unmatched');
                    }
              });
   if(self.menu.css('display')=='block')
                  {
                  self.menu.fadeOut(400);
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