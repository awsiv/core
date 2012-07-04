(function($){
    var element;
    module("eventTracker: methods", {
        setup:function(){
            $('body').append('<div id="event_viewer"></div>')
            element = $('#event_viewer').eventTrackerUI({baseUrl:'http://sudhir.no-ip.biz:8888/nova'});
        },
	teardown: function() {
		$("#event_viewer").remove();
                element.eventTrackerUI( "destroy" );
	}
   });
   
  /**
   *If the widget is wiped out from the memory and dom
   */
   
  test( "destroy", function() {
	expect( 1 );
	domEqual( "#event_viewer", function() {
		element.eventTrackerUI( "destroy" );
	});
  });
  
  /**
   * To get time stamp from the string supplied
   */
  
  test("getTimeStamp",function(){
      expect(1);
      var timestamp=element.eventTrackerUI('getTimeStamp','2012-06-28 14:00:00');
      equal(timestamp,1340892000,'must be valid  utc time stamp');
  });
  
  
  /**
   * To see if the function returns desired variables value as well as 
   * test Variable value
   */
  
  test('getProperty',function(){
      expect(3);
      var timers=element.eventTrackerUI("getProperty","timers");
      var tempEventDivs=element.eventTrackerUI("getProperty","tempEventDivs");
      var  onEditTracker=element.eventTrackerUI("getProperty","onEditTracker");
      
      deepEqual(timers,{}, "empty object");
      deepEqual(tempEventDivs,[], "empty Array");
      deepEqual(onEditTracker,{},'simple Empty object')
     
  })
  
  test('createTracker',function(){
      expect(9);
      var $tracker=element.eventTrackerUI('createTracker',{"id":"tracker","resource":"cfengine_correct_cftwin_files_libtwin","startTime":"2012-06-28 10:00:00","type":"not_kept"});
      equal($tracker.attr('id'),'tracker', "must have an id attribute with passed value");
      equal($tracker.attr('resource'),'cfengine_correct_cftwin_files_libtwin', "must have an resource attribute with passed value");
      equal($tracker.attr('startTime'),'2012-06-28 10:00:00', "must have an startTime attribute with passed value");
      equal($tracker.attr('type'),'not_kept', "must have an type attribute with passed value");
     
      equal($tracker.find('span.trackerStart').length,1, 'Must have a link to start Tracker with class trackerStart');
      equal($tracker.find('span.trackerDelete').length,1, 'Must have a link to delete Tracker');
      equal($tracker.find('span.trackerEdit').length,1, 'Must have a link to edit Tracker');
      
      equal($tracker.find('span.loadinggif').length,1, 'Must bgicon in each Tracker ');
      equal($tracker.find('span.loadinggif').css('display'),'none', 'BG icon must be hidden');
  });
  
  /**
   *To load the dummy create form 
   */
  test('loadcreateUI',function(){ 
      expect(1);
      $.mockjax({
         url: '/eventTracker/createView',
         proxy:'form.html'
      }); 
      
      var createPane=element.eventTrackerUI("getProperty","createPane"); 
      createPane.html('');
      element.eventTrackerUI("loadTrackerCreateUI");
      stop();  
      setTimeout(function() { 
             equal(createPane.find('form').length,1,'must have one form');
        start();  
      }, 1000);
     
  });
 
 /**
  * To see if the loading icon is displayed in the links after clicked
  */
  
  test('displayLoading',function(){
      //expect(1);
      var listPane=element.eventTrackerUI("getProperty","listPane");
     
  });
  
  /**
   * To refresh Create UI to reset it into original condition
   */
  
  test('refreshCreateUI',function(){
      var createPane=element.eventTrackerUI("getProperty","createPande");
      var $form=createPane.find('form');
      var $origanlform=$form.clone();
      $form.find('input[type=name]').val('testbundle');
      $form.find('input[type=submit]').val('anytext');
      $form.attr('action','/eventTracker/update');
      $form.find('input[type=hidden]').remove();
      createPane.find('#testBtn').find('span').text('Start');
      
      deepEqual($form, $origanlform, "must be exactly same")
      
      element.TrackerUI('refreshTrackerCreateUI');
      
      
  });
  
    
}(jQuery));





