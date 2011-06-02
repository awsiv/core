<div class="outerdiv grid_12">
          		
    <div class="innerdiv">
<!--<p><?php  echo "Logged in: $username" ; ?>&nbsp;<a href="<?php echo site_url('auth/logout'); ?>">Logout</a></p>-->
        <ul class="admin_menu">
            <li class="current"><a href="<?php echo site_url('auth/index');?>">Users</a></li>
            <li><a href="<?php echo site_url('auth/manage_group');?>">Groups</a></li>
            <li>Settings</li>
        </ul>
        <div class="holder">
            <div id="admin_content" class="tables">
            <?php  include 'user_list.php';?>
            </div>
        </div>
     </div>
</div>
<script src="<?php echo get_scriptdir()?>jquery.form.js" type="text/javascript"></script>
<script type="text/javascript">
$(document).ready(function() {
   var options = {
        target:  '#admin_content'  // target element(s) to be updated with server response
    };

//loading the create user page from server to add the user
   $('#add_user').live('click',function(event) {
       event.preventDefault();
       var path=$(this).attr('href');
       $("#admin_content").slideUp().load(path).slideDown();;
   });

//submitting the create user form
  $('#create_user').live('submit',function(event) {
       event.preventDefault();
      $(this).ajaxSubmit(options)
   });

//load the result from the server after delete page is called
  $('a.delete').live('click',function(event){
     event.preventDefault();
     var path=$(this).attr('href');
     $("#admin_content").load(path);
  });

//loading the edit page in admin_content of the admin area
  $('a.edit').live('click',function(event){
      event.preventDefault();
      var path=$(this).attr('href');
      $("#admin_content").load(path);
           
  });

//submitting the form ajaxically to the page in form action and loading the result in admin_content
  $('#edit_user').live('submit',function(event){
      event.preventDefault();
      $(this).ajaxSubmit(options);
  });

//loading all the pages in admin content after clicking the items in admin index menu
  $('.admin_menu li a').click(function(event){
      event.preventDefault();
      var path=$(this).attr('href');
      $("#admin_content").fadeOut(700).load(path).fadeIn(500);
      $(this).parent().addClass('current').siblings().removeClass('current');
  });

//loading the  group create page in the  admin area ajaxcially
  $('#add_group').live('click',function(event){
      event.preventDefault();
      var path=$(this).attr('href');
      $("#admin_content").slideUp().load(path).slideDown();
  });

//create a new group form the page loaded
 $('#create_group').live('submit',function(event){
      event.preventDefault();
      $(this).ajaxSubmit(options);
  });

 $('#Create_group').live('submit',function(event){
      event.preventDefault();
      $(this).ajaxSubmit(options);
  });


 $('#Update_group').live('submit',function(event){
      event.preventDefault();
      $(this).ajaxSubmit(options);
  });

});
</script>
