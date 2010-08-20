<?php foreach($policies as $policy) {?>
	<li>
		<span><?php echo $policy->policy_name ?></span>
		<a  href="#" id="recordsArray_<?php echo $policy->policy_id; ?>">edit</a>
	</li>
<?php }  ?>
