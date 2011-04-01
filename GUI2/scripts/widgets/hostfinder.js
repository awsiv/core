var hostfinder={
 _create: function() {
    // alert('notes created')
    },
 areas:{
    categories:"",
    resultpane:"",
    searchform:""
 },
 options: {
        url: "/widgets/hostfinder"
    },
 _init: function() {
        var self = this;
        this.element.click(function(event){
            event.preventDefault();
            self.createhostfinder();
            return false;
        });
    },
   createhostfinder:function()
    {
      self = this;
        // load the view and then save make a dialog out of it
        self.temp = $('<div style="display:hidden" title="Find Host" id="hostfinderctrl"></div>').appendTo('body');
         self.temp.load(self.options.url, {}, function(){
            self.areas.categories=self.temp.find('#searchby');
            self.areas.resultpane=self.temp.find('#result');
            self.areas.searchform=self.temp.find('#searchhost');
            self.temp.dialog({
                height: 'auto',
                width: 'auto',
                modal: true
            });
         });

         self.categories.find('li','click',self.categoryselected)
         self.temp.searchform.live('submit',searchsubmit());
    },
    categoryselected:function(event)
    {
        self = this;
        var selected_category=event.currentTarget.innerHTML;
        self.areas.searchform.attr("action","/widget/"+selected_category);
        self.aread.searchform.find("#searchinput").text('search by '+selected_category);
        if(selected_caterory=='class')
            {
                alert('selected');
            }
    },
   searchsubmit:function(event){
       event.preventDefault();
       self =this;
       var submit_url=self.temp.searchform.attr('action');
       var searchval=self.temp.searchform.find('input').val();
       $.ajax({
              type: "POST",
              url: submit_url,
              data: {value:searchval},
              dataType:"html",
              success: function(data) {
                self.updatesearchresult(data);
              }
            });
   },
   updatesearchresult:function(data){
     self=this;
     self.areas.resultpane.html(data);
   }

}
$.widget("ui.hostfinder", hostfinder);

