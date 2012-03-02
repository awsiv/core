(function($){
$.widget('ui.contextfinder',
{
 options: {
            baseUrl:'',
            width:500,
            height:300,
            allhost:true,
            autoOpen: false,
            hostkey:""
          },
_init: function(){
        var self=this;

},
_create:function(){
        var self=this;
            self.dialoginit();
        
        self.dialogcontent.dialog({
            height: self.options.height,
            width: self.options.width,
            autoOpen: false,
            modal: true
        });
        
        self.element.bind('click',function(event){
                event.preventDefault();
                self. elementtext=$(this).text();
                $(this).text('').append('<span class="loadinggif"> </span>');
               
                self.dialogcontent.dialog('open');
                self.loadpagebody(self.element.attr('href'));
            
            });
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
loadpagebody:function(url){
  var self=this;
    self.changeTitle('Loading');     
    submit_url=url;
        
    $.ajax({
        type: "POST",
        url: submit_url,
        data: {
               
        },
        dataType:"html",
        success: function(data) {

            self.updateDataInContainer(data);
            
            self.bindClassfinder();
            

        //clone items
        $(".line_wrapper").delegate('.add_line',"click",  function(event) {
            event.preventDefault();
            var new_el =  $(this).parent().clone();
            
            new_el.find('input').val('');
            
            new_el.appendTo('#myform');
            
            self.bindClassfinder();
            
        });
        
        //swap values
        $(".line_wrapper").delegate(".swap","click",  function(event) {
            event.preventDefault();    
            var tmp=$(this).parent().find('.include').val();
            $(this).parent().find('.include').val($(this).parent().find('.exclude').val());
            $(this).parent().find('.exclude').val(tmp)
        });

        $(".line_wrapper").delegate("#send","click",  function(event)  {
             event.preventDefault();
             //alert ( $('#send').prop("href"));
                 self._trigger("complete",null,{
                    inccl:self.getUrlParams()
                });
        });
    
            self.animate=false;
        },
        error:function(jqXHR, textStatus, errorThrown){
            var containerUlId = self.containerID();
            //self.dialogcontent.html($("<ul>").attr("id", self.containerID()));
            var li = '<li><span class="type">'+textStatus+'</span><p>'+errorThrown+'</p>';
            document.getElementById(containerUlId).innerHTML = li ;
            self.revertTitle();
        }
    });
},

bindClassfinder: function () {
       var self= this;
       $('.class_selector',$('#contentfindercontainer')).classfinder({
        defaultbehaviour:false,
        
        complete:function(event,data)
        {
          $(this).parent().find('input').val(data.selectedclass);

            var url = self.getFinderUrl();
            // change URL for all classfinder buttons
            $(this).parent().find('a.class_selector').prop("href", url); 
            
            //change send url
            $('#send').prop("href", url); 
        }
    });
},
getFinderUrl: function() {
            var self= this;

            // constuct url
            var url = self.options.baseUrl + '/widget/allclasses?';
            
            url =  url + self.getUrlParams();
            
            return url;
},

getUrlParams: function () {
     var self= this;
            //prepare values for finders url
            var includes = new Array();
            var excludes = new Array();
           
            $('input[name="include[]"]').each(function(index)
            {
                if ($(this).val() != '')
                {
                    includes.push($(this).val());
                }
            });
            
            $('input[name="exclude[]"]').each(function(index)
            {
                if ($(this).val() != '')
                {
                   excludes.push($(this).val());
                }
            });            
          
            // constuct url
            var url = '';
            
            if (includes.length > 0)
            {
                url = url + 'includes=' + encodeURIComponent(includes) + '&';
            }
            
            if (excludes.length > 0) 
            {
                url = url + 'excludes=' + encodeURIComponent(excludes);
            }
            return url;
},

dialogContainer: function() {
            var existing = $("#contentfindercontainer");
            if ( existing.size() > 0) {
                return existing.first();
            }
            else {
                //single shared element for modal dialogs
                var requestDialog = $('<div id="contentfindercontainer" style="display:none" class="result" title="Contextfinder"></div>').appendTo('body').
                dialog({
                    autoOpen: false
                });
                return requestDialog;
            }
        },
        changeTitle:function(text) {
            var self = this;
            $('#'+self.containerID()).dialog('option', 'title', text);
            self.ajaxloader.show();
        },
        
        revertTitle:function() {
            var self = this;
            $('#'+self.containerID()).dialog('option', 'title', self.originalTitle);
             self.ajaxloader.hide();
        },
         containerID:function() {
            return this.element.attr('id');
        },
        updateDataInContainer:function(data) {
            var self = this
              // To end the scrolling so that no further request is sent
            if (data.length == 0 || data ==null ) {
                self.scrollingEnd = true;
                self.revertTitle();
                return;
            }
            
            var containerUlId = self.containerID();
            document.getElementById('contentfindercontainer').innerHTML = data;

            
            self.dataLoaded = true;            
            self.element.text(self.elementtext);
            self.revertTitle();                

        },
   
 destroy: function(){
            // remove this instance from $.ui.mywidget.instances
            var element = this.element,
                    position = $.inArray(element, $.ui.contextfinder.instances);
            // if this instance was found, splice it off
            if(position > -1){
                    $.ui.contextfinder.instances.splice(position, 1);
            }
            // call the original destroy method since we overwrote it
            $.Widget.prototype.destroy.call( this );
        }

});

$.extend($.ui.contextfinder, {
		instances: []
	});

})(jQuery);