(function($){
$.widget('ui.bundlefinder',
{
options: {
            filterhandlerurl: "/widget/filterclass",
            defaultbehaviour:true,
            width:700,
            height:600

          },
_init: function(){

},
_create:function(){
        var self=this;
        
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
 destroy: function(){
			// remove this instance from $.ui.mywidget.instances
			var element = this.element,
				position = $.inArray(element, $.ui.dialogContainer.instances);
			// if this instance was found, splice it off
			if(position > -1){
				$.ui.dialogContainer.instances.splice(position, 1);
			}
			// call the original destroy method since we overwrote it
 }
 
});
$.extend($.ui.bundlefinder, {
		instances: []
	});

})(jQuery);

