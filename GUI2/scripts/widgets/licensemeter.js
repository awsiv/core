$.widget('ui.licensemeter', $.extend({}, $.ui.progressbar.prototype,
{
_init: function(){
		$.ui.progressbar.prototype._init.call(this); // call the original function
		//this._mouseInit(); // start up the mouse handling
},
_create:function(){
        var self=this;
        $.ui.progressbar.prototype._create.call(this);
        self.displaytext();


},
displaytext:function(){
   //console.log(this.value());
   //console.log(this);
   var value=$('<span>');
   //var startdate=$('<span class="startdate">');
   //var expirydate=$('<span class="enddate">');
   this.element.addClass('textalignright');
   value.text(this.options.daysleft+" Days remaining").appendTo(this.element);
   
   //startdate.text("dec 27").appendTo(this.element);
   //expirydate.text("jan 27").appendTo(this.element);

   if(this.value() >= 95)
       {
         this.valueDiv.css({'background':'#D8000C'})
       }
}

}));


