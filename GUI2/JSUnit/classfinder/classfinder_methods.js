(function($){
    var element;
    module("classfinder: methods", {
        setup:function(){
            element = $("#classes").classfinder();
        },
	teardown: function() {
		$("body>.ui-dialog").remove();
                element.classfinder( "destroy" );
	}
   });
   
  test( "destroy", function() {
	expect( 1 );
	domEqual( "#classes", function() {
		element.classfinder( "destroy" );
	});
  });
  
  test("isopen", function(){
    expect( 2 );        //number of expectation and assertions
    equal($('.ui-dialog').eq(0).css('display'),'none','dialog should not be displayed when binded');
    $('#classes').click();
    equal($('.ui-dialog').eq(0).css('display'),'block' , 'should be displayed when clicked');
   
  });
  
  test('classesloaded',function(){
     expect(7)             //number of expectation
     $.mockjax({
     url: '/widget/allclasses',
     responseTime: 750,
     responseText: ['firstclass','secondClass']
    }); 
     equal($('.classList').children().length,0,"Should contain zero elements initally");
     element.classfinder("loadpagebody");
     stop();
     
     setTimeout(function() {
     equal($('.classList').children().length,2,"Should contain two elements");
     var items = $(".classList").children();
     equal(items.eq(0).children('a').eq(0).text(), "firstclass", "first named firstclass and must be of type a");
     
     equal(items.eq(0).children('a').eq(1).text().toLowerCase(), "view hosts", "must have a view host button");
     equal(items.eq(0).children('a').eq(1).attr('class'), "action btn", "ensure that the button has class action and button");
    
     equal(items.eq(0).children('a').eq(2).text().toLowerCase(), "add to list", "add to list button should be present");
     equal(items.eq(0).children('a').eq(2).attr('class'), "classadd btn", "ensure that the button has class action and button");
     start();
     }, 1000);
    
  });
 
 /* test("loadpagebody",function(){
    
     $.mockjax({
     url: '/widget/allclasses',
     responseTime: 750,
     responseText: ['firstclass','secondClass']
    });
    
    stop();
    element.classfinder("loadpagebody");
    start();
    var container=element.classfinder('getContainer');
    $('#'+container).css('display', 'block');
    console.log($('#'+container).css('display'));
    console.log($('.classList'));
  });*/
    
}(jQuery));


