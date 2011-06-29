<?php  if (!defined('APPPATH')) exit('No direct script access allowed');

/*
 * @Author: Dean Ericson
 * @Email: mail@deanericson.com
 *
 */
final class BreadCrumbList{
	private $breadCrumbs;
	private $CI;
	private $delimiter;
	function BreadCrumbList(){
		$this->CI =& get_Instance();
		$delimiter = $this->CI->config->item('breadcrumbDelimiter');

		if(!empty($_SESSION['breadCrumbList'])){
			$s1 = $_SESSION['breadCrumbList'];
			$this->breadCrumbs = $s1;
		}else{
			$this->breadCrumbs = array();
		}
		if(!isset($delimiter)){
			$this->setDelimiter('&rarr;');
		}else{
			$this->setDelimiter($delimiter);
		}

	}
	function reset(){
		$this->breadCrumbs = array();
	}
	function add($bc){
			$tmpArr = array();
			if($bc->isRoot()){
				$this->reset();
			}
                        if($bc->replaceExisting())
                        {
                            if(!$this->replace_last_with_current($bc->getTitle(), $bc->getUrl())){
                                $this->breadCrumbs[] = $bc;
				$_SESSION['breadCrumbList']=$this->breadCrumbs;
                            }
                            return;
                        }
                        if(!in_array($bc, $this->breadCrumbs)){
				$this->breadCrumbs[] = $bc;
				$_SESSION['breadCrumbList']=$this->breadCrumbs;
			}
                        else{
				// in array, remove objects after index
				$index = array_search($bc,$this->breadCrumbs);
				for($i=(count($this->breadCrumbs)-1); $i>=($index+1); $i-- ){
					unset($this->breadCrumbs[$i]);
				}
				$_SESSION['breadCrumbList']=$this->breadCrumbs;
			}
	}
	function display(){
		$iter = $_SESSION['breadCrumbList'];
		$i=1;
		$return_str='';
		foreach($iter as $bc){
			$uBC = $bc;
			$del = $i < count($iter)?$this->delimiter:'';
			if($i < count($iter)){
                                if ($uBC->isRoot())
                                 $return_str .= "<li>".anchor($uBC->getUrl(),$uBC->getTitle(), array('class'=>'activeBreadCrumbs'))."<span class=\"front\"></span></li>";
                                else
                                 $return_str .= "<li><span class=\"back\"></span>".anchor($uBC->getUrl(),$uBC->getTitle(), array('class'=>'activeBreadCrumbs'))."<span class=\"front\"></span></li>";
                               
			}else{
				//$return_str .= "<li><span class=\"current\">".$uBC->getTitle().'</span> '.$del."</li>";
                                 //$return_str .= "<li>".anchor($uBC->getUrl(),$uBC->getTitle(), array('class'=>'current')).' '.$del."</li>";
                                   if ($uBC->isRoot())
                                     $return_str .= "<li>".anchor($uBC->getUrl(),$uBC->getTitle(), array('class'=>'current'))."<span class=\"front\"></span></li>";
                                   else
                                    $return_str .= "<li><span class=\"back\"></span>".anchor($uBC->getUrl(),$uBC->getTitle(), array('class'=>'current'))."<span class=\"front\"></span></li>";
                               
			}
		$i++;
		}
		return $return_str;
	}
	function getBreadCrumbs(){
		return $this->breadCrumbs;
	}
	function setBreadCrumbs($b){
		$this->breadCrumbs=$b;
	}
	function getCI(){
		return $this->CI;
	}
	function setCI($c){
		$this->CI=$c;
	}
	function getDelimiter(){
		return $this->delimiter;
	}
	function setDelimiter($d){
		$this->delimiter='<span class="delimiter" >'.$d.'</span>';
	}
        
        function checkurl($url){
              $newbreadbrumbs=$this->breadCrumbs;
              $index=0;
               foreach ($this->breadCrumbs as $bc)
               {
                   if($bc->getUrl() ==$url)
                   {
                     for($i=(count($newbreadbrumbs)-1); $i>=($index+1); $i-- ){
			   unset($newbreadbrumbs[$i]);
		      }
                   }
                  $index++;
               }
               $this->breadCrumbs=$newbreadbrumbs;
               $_SESSION['breadCrumbList']=$newbreadbrumbs;
        }

   function replace_last_with_current($checkstring,$replaceurl){
            $breadcrumbs = $this->getBreadCrumbs();
            $modified = false;
            $index=0;
            foreach ($breadcrumbs as $breadcrumb) {
               // $pos = strpos($breadcrumb->getUrl(), $checkstring);
               // var_dump($pos);
                //echo $pos;
                //if ($pos!== FALSE ) {
                if(strcmp(strtolower($breadcrumb->getTitle()), strtolower($checkstring)) == 0){
                    $breadcrumb->setUrl($replaceurl);
                    $modified = true;
                    break;
                }
                $index++;
            }
          for($i=(count($this->breadCrumbs)-1); $i>=($index+1); $i-- ){
	    unset($this->breadCrumbs[$i]);
	  }
	$_SESSION['breadCrumbList']=$this->breadCrumbs;
        return $modified;
       }
}
?>
