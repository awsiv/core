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
        height:'600',
        width:'700'
    },
 _init: function() {
     var self=this;
     self.cfui.searchform.find('input[type="text"]').focusout();
    },
   createhostfinder:function()
    {
      var self = this;
        // load the view and then save make a dialog out of it
        self.temp = $('<div style="display:hidden" title="Find Host" id="hostfinderctrl"></div>').appendTo('body');
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
         self.cfui.categories.delegate('li','click',{ui:self},self.categoryselected)
         self.temp.delegate('form','submit',{ui:self},self.searchsubmit);
         self.element.bind('click',function(event){event.preventDefault();self.temp.dialog('open')});
         self.cfui.searchform.delegate('input[type="text"]','focusin',self.searchboxevent);
         self.cfui.searchform.delegate('input[type="text"]','focusout',self.searchboxevent);
         self.cfui.searchform.find('input[type="text"]').data('default',self.cfui.searchform.find('input[type="text"]').val());
    },

    categoryselected:function(event)
    {
        var selected_category=$(this).text().toLowerCase();
        var self=event.data.ui;
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
   },
   
   updatesearchresult:function(data){
     //self.cfui.resultpane.html(data);
   },

   createclasstagcloud:function(ui){
       var self=ui;
        self.classdlg = $('<div style="display:hidden" title="classes" id="tagCloud"></div>').appendTo(self);
        $.getJSON(self.options.classhandler, function(data) {
               $("<ul>").attr("id", "tagList").appendTo(self.classdlg);
                  $.each(data, function(i, val) {
                        var li = $("<li>");
                        $("<a>").text(val[0]).attr({title:val[0], href:"#"}).appendTo(li);
                        li.children().css("fontSize",  val[1] + "em");
                        li.appendTo("#tagList");
                     });
        });
        self.classdlg.dialog({
                     height: self.options.height,
                     width: 'auto',
                     modal: true,
                     close: function(event, ui) {
                    self.classdlg.remove();
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
      if(this.value==$(this).data('default') && event.type=='focusin')this.value='';
      if(this.value=='' && event.type=='focusout')this.value=$(this).data('default');
    }


}
$.widget("ui.hostfinder", hostfinder);

