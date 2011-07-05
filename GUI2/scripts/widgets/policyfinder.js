(function($){
$.widget('ui.policyfinder',
{
 options: {
            baseUrl: '',
            filterhandlerurl: "/widget/filterpolicy",
            width:700,
            height:600

          },
_init: function(){

},
_create:function(){
        var self=this;
        self.addsearchbar();
        self.addalphapager();
        $.ui.policyfinder.instances.push(this.element);
},
dialogContainer: function() {
            var existing = $("#policylistcontainer");
            if ( existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="policylistcontainer" style="display:none" class="result" title="Policies"></div>').appendTo('body').
                dialog({
                    autoOpen: false
                });
                return requestDialog;
            }
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

  self.searchbar=$('<form id="policyfindersearch" action="'+self.options.baseUrl+'/widget/search_by_bundle"><span class="search"><input type="text" name="search" value="search by bundle"/></span></form>')
  self.titlebar.append(self.searchbar).delegate('form','submit',$.proxy(self.searchpolicyfile,self));
  self.searchbar.delegate('input[type="text"]','click',function(){$(this).focus().select()});
  self.searchbar.delegate('input[type="text"]','focusin',$.proxy(self.searchboxevent,self));
  self.searchbar.delegate('input[type="text"]','focusout',$.proxy(self.searchboxevent,self));
  self.searchbar.find('input[type="text"]').data('default',self.searchbar.find('input[type="text"]').val());
  self.searchbar.delegate('input[type="text"]','keyup',$.proxy(self.searchbarkeyevent,self));

        self.menu=$('<div class="categories"><ul id="classoptions"></ul></div>');
        self.menu.find('ul').append('<li>by bundle</li><li>by handle</li><li>by promiser</li>');
        $('<span class="slider"></span>').appendTo(self.menu).bind('click',function(event){ self.menu.slideUp();});
        self.menu.appendTo(self.titlebar).hide();
        self.menu.delegate('li','click',$.proxy(self.menuitemclicked,self));
        self.element.bind('click',function(event){
            event.preventDefault();
            self.dialogcontent.dialog('open');
            if(!$("#policyList").size()>0)
                {
                 self.dialogcontent.html(self.ajaxloader);
                 self.loadpagebody(self.element.attr('href'),"",true);
                }
        });

},

menuitemclicked:function(event){
  var self=this;
  var sender=$(event.target);
  var selected_category=sender.text().toLowerCase();
  self.searchbar.find('input[type="text"]').val('search '+sender.text().toLowerCase()).data('default','search '+sender.text().toLowerCase())
  self.searchbar.attr("action",self.options.baseUrl+"/widget/search_"+selected_category.replace(/\s+/g, "_").toLowerCase());
  self.dialogcontent.html(self.ajaxloader);
  self.loadpagebody(self.searchbar.attr('action'),"",false) ;
  self.searchbar.find('input[type="text"]').trigger('blur');
  self.alphasearch.find('li').removeClass('selected');
  sender.addClass('selected').siblings().removeClass('selected');
  //self.menu.fadeOut();
},

loadpagebody:function(url,val,escreg){
  var self=this,
      submit_url=url,
      searchval=val;
      $.ajax({
                  type: "POST",
                  url: submit_url,
                  data: {filter:searchval,reg:escreg},
                  dataType:"json",
                  success: function(data) {
                  self.dialogcontent.html($("<ul>").attr("id", "policyList"));
                  $.each(data, function(i, val) {
                                        var li = $("<li>");
                                        //$("<a>").attr({title:val, href:"/promise/details/"+val[0]})
                                        //.append('<span class="type">'+val[3]+'</span><p><span class="promiser">'+val[4]+'</span><span class="handle">'+val[0]+'</span> / <span class="bundle">'+val[2]+'</span></p><p class="clearleft"></p>')
                                        //.appendTo(li);
                                        
                                        li.append('<span class="type">'+val[3]+'</span>');
                                        var p =$("<p>")
                                        $("<a>").attr({title:"promise : "+val, href:self.options.baseUrl+"/promise/details/"+escape(val[0])}).addClass('promiselnk').append('<span class="promiser">'+val[4]+'</span>').appendTo(p);
                                        $("<a>").attr({title:"handle : "+val, href:self.options.baseUrl+"/promise/details/"+escape(val[0])}).append('<span class="handle">'+val[0]+'</span>').appendTo(p);
                                        $("<a>").attr({title:"bundle : "+val, href:self.options.baseUrl+"/bundle/details/bundle/"+escape(val[2])+"/type/"+val[3]}).append('<span class="bundle">'+val[2]+'</span>').appendTo(p);
                                        p.appendTo(li);
                                        li.appendTo("#policyList");
                                  });
                  }
           });
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
 searchpolicyfile:function(event)
    {
       event.preventDefault();
       var self=this,
           sender=$(event.target),
           submit_url=sender.attr('action'),
           searchval=sender.find('input').val();
           self.loadpagebody(submit_url,searchval,true);
           self.menu.fadeOut();
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
   self.loadpagebody(self.searchbar.attr('action'),"^"+$(event.target).text(),false);
   if(self.menu.css('display')=='block')
                  {
                  self.menu.fadeOut(400);
                  }
  },

  searchbarkeyevent:function(event){
     var self = this,
        searchbox= $(event.target),
        searchWord = searchbox.val();
      if(searchWord==''&& event.keyCode == 8)
          {
              self.menu.slideDown();
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

$.extend($.ui.policyfinder, {
		instances: []
	});

})(jQuery);


