<?php if (!defined('BASEPATH')) exit('No direct script access allowed');
class Db_policy extends Model {

	const TABLE	= 'policy';
	function Db_policy()
	{
		parent::Model();	
	}
	
	function add_policy($data)
	{
		if($this->db->insert(self::TABLE, $data))
				return($this->db->insert_id());
			else
				return false;
	}
	
	function update_policy($pid,$data)
	{
		$this->db->where('policy_id',$pid);
		$this->db->update(self::TABLE,$data);
		return $this->db->affected_rows() > 0;
	}
	
	function get_policies()
	{
		$query = $this->db->get(self::TABLE);
		return $query->result();
	}
	
	function get_policy_details($pid)
	{
		$this->db->where('policy_id', $pid);
		$query = $this->db->get(self::TABLE);
		if ($query->num_rows() == 1) return $query->row();
		return NULL;
	}
}

?>
