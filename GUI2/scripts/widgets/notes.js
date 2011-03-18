var notes  = {
    _create: function() {
    // alert('notes created')
    },
    _init: function() {
        var self = this;
        this.element.click(function(event){
            self.initialize();
            event.preventDefault();
            return false;
        });
        
    },
    showRequest: function (formData, jqForm, options) {
        // formData is an array; here we use $.param to convert it to a string to display it
        // but the form plugin does this for you automatically when it submits the data
        var queryString = $.param(formData);

        // jqForm is a jQuery object encapsulating the form element.  To access the
        // DOM element for the form do this:
        // var formElement = jqForm[0];

        //alert('About to submit: \n\n' + queryString);

        // here we could return false to prevent the form from being submitted;
        // returning anything other than false will allow the form submit to continue
        return true;
    } ,
    showResponse: function(responseText, statusText, xhr, $form)  {
        // for normal html responses, the first argument to the success callback
        // is the XMLHttpRequest object's responseText property

        // if the ajaxSubmit method was passed an Options Object with the dataType
        // property set to 'xml' then the first argument to the success callback
        // is the XMLHttpRequest object's responseXML property

        // if the ajaxSubmit method was passed an Options Object with the dataType
        // property set to 'json' then the first argument to the success callback
        // is the json data object returned by the server
        

        //alert('status: ' + statusText + '\n\nresponseText: \n' + responseText +
          //  '\n\nThe output div should have already been updated with the responseText.');
          console.log(responseText);
         $('#notes-table tbody #no-data-row',self.temp).remove();
         $('form textarea',self.temp).val('');

        $('#notes-table tbody',self.temp).append(responseText.html);

        // change url
        var newUrl = '/notes/index/action/show/nid/'+responseText.nid;
        self.element.attr('href',newUrl);

    } ,

     addError: function (jqXHR, textStatus, errorThrown)
    {
            
        alert(jqXHR.responseText);
        
    },
    _refresh: function() {
        self.temp.load(self.url, {}, function(){
            self.temp.dialog({
                height: 'auto',
                width: 'auto',
                modal: true
            });
            $('#notes-form',self.temp).submit(function(e) {
                e.preventDefault();
                // inside event callbacks 'this' is the DOM element so we first
                // wrap it in a jQuery object and then invoke ajaxSubmit
                var aoptions = {
                    target:        '',   // target element(s) to be updated with server response
                    beforeSubmit:  self.showRequest,  // pre-submit callback
                    success:       self.showResponse,  // post-submit callback
                    error: self.addError,
                    dataType:  'json'        // 'xml', 'script', or 'json' (expected server response type)
              
                // other available options:
                //url:       url         // override for form's 'action' attribute
                //type:      type        // 'get' or 'post', override for form's 'method' attribute
                //clearForm: true        // clear all form fields after successful submit
                //resetForm: true        // reset the form after successful submit

                // $.ajax options can be used here too, for example:
                //timeout:   3000
                };

                $(this).ajaxSubmit(aoptions);

                // !!! Important !!!
                // always return false to prevent standard browser submit and page navigation
                return false;
            });

            
        
           
        });
        return false;


    },

    initialize: function() {
        self = this;
        self.url = this.element.attr('href');
      
        // load the view and then save make a dialog out of it 
        self.temp = $('<div style="display:hidden" title="Notes"></div>').appendTo('body');
        self._refresh();

    }
};
$.widget("ui.notes", notes); // create the widget