<?php
/*
 * Extended from core CI to adjust the css look of validation errors
 */
if ( ! function_exists('validation_errors'))
{
	function validation_errors($prefix = '', $suffix = '')
	{
		if (FALSE === ($OBJ =& _get_validation_object()))
		{
			return '';
		}
                if($OBJ->error_string($prefix, $suffix)<>'')
                {
		return '<p class="error">'.$OBJ->error_string($prefix, $suffix).'</p>';
                }
                else
                {
                return $OBJ->error_string($prefix, $suffix);    
                }
	}
}
?>
