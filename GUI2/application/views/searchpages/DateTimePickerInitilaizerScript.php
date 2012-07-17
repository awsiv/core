<script type="text/javascript">
    $(document).ready(function(){
        var fromTextField= $('input:text[name=fromText]');
        var toTextField=$("input:text[name=toText]");
        
        var fromTimeStamp=$('input:hidden[name=from]');
        var toTimeStamp=$('input:hidden[name=to]');
        
        if(fromTimeStamp.val()!=''){
            fromTextField.val(common.getLocalTimeStringFromUTC(fromTimeStamp.val()));
        }
        
       if(toTimeStamp.val()!=''){
            toTextField.val(common.getLocalTimeStringFromUTC(toTimeStamp.val()));
       }
       
         
        fromTextField.datetimepicker({
            dateFormat:"yy-mm-dd",
            onClose: function(dateText, inst) {
                var endDateTextBox = toTextField;
                if (endDateTextBox.val() != '') {
                    var StartDate = common.getTimeStamp(dateText)
                    var EndDate = common.getTimeStamp(endDateTextBox.val());
                    if (StartDate > EndDate)
                        endDateTextBox.val(dateText);
                }
                else {
                    endDateTextBox.val(dateText);
                }
                fromTimeStamp.val(common.getTimeStamp($(this).val(),'utc'));
            },
            onSelect: function (selectedDateTime){
                toTextField.datetimepicker('option', 'minDate', new Date(common.getTimeStamp(selectedDateTime)));
                toTextField.datetimepicker('option', 'minDateTime', new Date(common.getTimeStamp(selectedDateTime)));
            }
        });
        
        
        toTextField.datetimepicker({
        dateFormat:"yy-mm-dd",
        onClose: function(dateText, inst) {
            var startDateTextBox = fromTextField;
            if (startDateTextBox.val() != '') {
                var StartDate = common.getTimeStamp(startDateTextBox.val());
                var EndDate = common.getTimeStamp(dateText);
                if (StartDate > EndDate)
                    startDateTextBox.val(dateText);
            }
            else {
                startDateTextBox.val(dateText);
            }
           toTimeStamp.val(common.getTimeStamp($(this).val(),'utc'));
        },
        onSelect: function (selectedDateTime){
            //var end = $(this).datetimepicker('getDate');
            fromTextField.datetimepicker('option', 'maxDate', new Date(common.getTimeStamp(selectedDateTime)));
            fromTextField.datetimepicker('option', 'maxDateTime', new Date(common.getTimeStamp(selectedDateTime)));
        }
       });
       
    if(fromTextField.val()!=''){
        toTextField.datetimepicker('option', 'minDate',  new Date(common.getTimeStamp(fromTextField.val())));
        toTextField.datetimepicker('option', 'minDateTime',  new Date(common.getTimeStamp(fromTextField.val()))); 
    }
   
    if(toTextField.val()!=''){
        fromTextField.datetimepicker('option', 'maxDate', new Date(common.getTimeStamp(toTextField.val())));
        fromTextField.datetimepicker('option', 'maxDateTime', new Date(common.getTimeStamp(toTextField.val()))); 
     }
     
        $('#searchform').find('form').submit(function(){ 
            toTimeStamp.val(common.getTimeStamp(toTextField.val(),'utc'));
            fromTimeStamp.val(common.getTimeStamp(fromTextField.val(),'utc'));
        });
        
   });

</script>
