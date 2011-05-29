<?php
if(isset ($error))
{
    ?>
 <span>Error:<?php echo $error ?></span>
<?php
}else{
 require_once('approved_policies_table.php');
 echo anchor('/repository/approvedPolicies', 'view more..', array('target'=>"_self"));
}
?>

