var hostfinder={
 _create: function() {
    // alert('notes created')
     var self = this;
     self.createhostfinder();
    },
 cfui:{
    categories:"",
    resultpane:"",
    searchform:""
 },
 options: {
        url: "/widget/hostfinder",
        classhandler:"/widget/cfclasses",
        height:525,
        width:700
    },
 _init: function() {
     var self=this;
     //self.cfui.searchform.find('input[type="text"]').focusout();
     self.cfui.categories.hide();
     self.cfui.searchform.find('input[type="text"]').focusout();
    },
   createhostfinder:function()
    {
      var self = this;
        // load the view and then save make a dialog out of it
        self.temp = $('<div style="display:hidden" title="Find Host" id="hostfinderctrl"></div>').appendTo('body');
        self.ajaxloader=$('<div class="loading"></div>');
        $.ajax({
              type: "POST",
              url: self.options.url,
              data: {},
              dataType:"html",
              async:false,
              success: function(data) {
                  self.temp.html(data);
                  self.cfui.categories=self.temp.find('#searchby');
                  self.cfui.resultpane=self.temp.find('#searchresult');
                  self.cfui.searchform=self.temp.find('#searchhost');
                  self.$filter=$('#filters').find('ul');
                  self.temp.dialog({
                     height: self.options.height,
                     width: 'auto',
                     autoOpen: false,
                     modal: true
                     });   
              }
            });

          //console.log( self.areas.categories);
         self.temp.parent().addClass('customdlg').removeClass('ui-widget-content');
         self.titlebar=self.temp.siblings('div.ui-dialog-titlebar');
         self.titlebar.append(self.cfui.searchform).delegate('form','submit',{ui:self},self.searchsubmit);

         self.cfui.categories.addClass('categories').appendTo( self.titlebar).hide();

         self.temp.siblings('div.ui-dialog-titlebar')
         self.cfui.categories.delegate('li','click',{ui:self},self.categoryselected)
         //self.temp.delegate('form','submit',{ui:self},self.searchsubmit);
         self.element.bind('click',function(event){event.preventDefault();self.temp.dialog('open')});
         self.temp.find('#aplhaPaging').delegate('li','click',{ui:self},self.sortalphabetically);
         self.cfui.searchform.delegate('input[type="text"]','click',function(){$(this).focus()});
         self.cfui.searchform.delegate('input[type="text"]','focusin',{ui:self},self.searchboxevent);
         self.cfui.searchform.delegate('input[type="text"]','focusout',{ui:self},self.searchboxevent);
         self.cfui.searchform.find('input[type="text"]').data('default',self.cfui.searchform.find('input[type="text"]').val());
    },

    categoryselected:function(event)
    {
        var selected_category=$(this).text().toLowerCase();
        var self=event.data.ui;
        self.filtermethod=selected_category;
        self.cfui.searchform.attr("action","/widget/search_by_"+selected_category.replace(/\s+/g, "").toLowerCase());
        self.cfui.searchform.find('input[type="text"]').val('search by '+selected_category).data('default','search by '+selected_category);
        if( $(this).attr('id')=='search_class')
            {
              self.createclasstagcloud(self);
            }
       
    },
    
   searchsubmit:function(event){
       event.preventDefault();
       var submit_url=$(this).attr('action');
       var searchval=$(this).find('input').val();
       var self=event.data.ui;
       self.cfui.categories.slideUp();
       if(self.filtermethod != "class"){
           self.cfui.resultpane.html(self.ajaxloader);
           $.ajax({
                  type: "POST",
                  url: submit_url,
                  data: {value:searchval},
                  dataType:"html",
                  success: function(data) {
                    //self.updatesearchresult(data);
                    self.cfui.resultpane.html(data);
                  }
           });
       }
       
   },
   
   updatesearchresult:function(data){
     //self.cfui.resultpane.html(data);
   },

   createclasstagcloud:function(ui){
       var self=ui;
        self.classdlg = $('<div style="display:hidden" title="classes" id="tagCloud"></div>').appendTo(self);
        self.classdlg.dialog({
                     height: self.options.height,
                     width: 700,
                     modal: true,
                      position:'center',
                     close: function(event, ui) {
                       self.classdlg.remove();
                     },
                     open:function(event,ui){
                         $(this).html(self.ajaxloader.clone().css({'margin-top':$(this).height()/2}));
                         
                         $.getJSON(self.options.classhandler, function(data) {
                             self.classdlg.html($("<ul>").attr("id", "tagList"));
                               //$("<ul>").attr("id", "tagList").appendTo(self.classdlg);
                                  $.each(data, function(i, val) {
                                        var li = $("<li>");
                                        $("<a>").text(val[0]).attr({title:val[0], href:"#"}).appendTo(li);
                                        li.children().css("fontSize",  val[1]/10+1 + "em");
                                        li.appendTo("#tagList");
                                  });

                        });
                     }
                     });
        self.classdlg.delegate('a', 'click',{ui:self},self.addclassfilter);
   },

    addclassfilter:function(event)
    {
     event.preventDefault();
     var self=event.data.ui
     var selectedclass= $(this).text();
     //self._trigger("complete",null,{selectedclass:curclass});
     
     self.classdlg.dialog('close');
      var li = $("<li>");
      li.text(selectedclass).data('filter',selectedclass).appendTo(self.$filter);
                       $("<a>").text("X").appendTo(li)
                       var filters=new Array();
                               self.$filter.find('li').each(function(index) {
                                    filters.push($(this).data('filter'));
                               });
     self.cfui.resultpane.load('/widget/ajaxlisthost/',{'filter':filters},function(){});
     self.$filter.find('li').delegate('a','click',{ui:self},self.removeclassfilter);
    },

    removeclassfilter:function(event)
    {
        event.preventDefault();
        var self=event.data.ui
         $(this).parent().remove();
                              var filters=new Array();
                               self.$filter.find('li').each(function(index) {
                                    filters.push($(this).data('filter'));
                               });
        self.cfui.resultpane.load('/widget/ajaxlisthost/',{'filter':filters},function(){});
    },

    searchboxevent:function(event)
    {
      var self=event.data.ui
      if(this.value==$(this).data('default') && event.type=='focusin')
          {
              self.cfui.categories.slideDown();
              this.value='';
          }
      if(this.value=='' && event.type=='focusout')
          {
              this.value=$(this).data('default');
          }
    },

    sortalphabetically:function(event)
    {
      var self=event.data.ui
      //alert($(this).text());
      self.cfui.resultpane.load('/widget/search_by_hostname',{'value':$(this).text()},function(){});
    }

}
$.widget("ui.hostfinder", hostfinder);

