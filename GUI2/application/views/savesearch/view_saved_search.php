<div style="min-width: 500px;">
    <?php if (is_array($group) && !empty($group))  { foreach ((array)$group as $index=>$g) { ?>
        <table class="bundlelist-table">
            <thead>
                <tr><th scope="blue" colspan="4"><?php echo $index; ?></td></tr>
                <tr>
                    <th scope="col">Search Label</th>
                    <th scope="col">Date</th>
                    <th scope="col"></th>
                    <th scope="col"></th>
                </tr>
            </thead>
            <tbody>
                <?php if (!empty($g)) {
                    ?>
                    <?php foreach ((array) $g as $search) {
                        ?>
                        <tr>
                            <td><?php echo $search->getLabel(); ?></td>
                            <td><?php echo $search->getDate(); ?></td>
                            <td>
                                <a href="<?php echo site_url() . "/search/index/" . $search->getUrl(); ?>" target="_self"> Run this search</a>
                            </td>
                            <td>
                                <a class="jDelete" href="<?php echo site_url() . "/savedsearch/delete/" . $search->getId(); ?>" target="_self">delete this search</a>
                            </td>
                        </tr>
                    <?php } ?>
                <?php } else {
                    ?>
                    <tr id="no-data-row">
                        <td colspan="3">No saved searches found.</td>
                    </tr>
                <?php } ?>
            </tbody>
        </table>
    <?php } } else { ?>
    <div class="info">No saved search found.</div>
    <?php } ?>
</div>
<script type="text/javascript">

$('a.jDelete').click(function(e) {
    e.preventDefault();
    if (!confirm('Are you sure to delete this search?')) {
        return false; 
        
    }
    
    var url = $(this).attr('href');
  
    var parent = $(this).parent().parent();
    var td =  $(this).parent();
    $.ajax({
      type: 'get',
      url: url,
      beforeSend: function() {
        td.animate({'backgroundColor':'#fb6c6c'},400);
      },
      success: function() {
        parent.slideUp(300,function() {
          parent.remove();
        });
      },
      error:function(){
          alert('Error while deletion.')
      }
    }); 
    return false;
}) 


</script>