<?php  if (!defined('APPPATH')) exit('No direct script access allowed');

@session_start();

/*
 * @Author: Dean Ericson
 * @Email: mail@deanericson.com
 *
 */

final class BreadCrumb{
	private $title;
	private $url;
	private $isRoot;
        private $replace_if_exist;

	/*
	 * No need to set anything up.  I autoload it and
	 * let the setters mutate the properties through the
	 * setBreadCrumb function
	 */
	function BreadCrumb(){
          
        }

	/*
	 * Takes an array which represents this class (BreadCrumb)
	 * as a parameter. Sets the class through it mutators,
	 * then sets itself within the BreadCrumbList Object.
	 */
	function setBreadCrumb($bcObjArr){
		$this->title=$bcObjArr['title'];
		$this->url=$bcObjArr['url'];
		$this->isRoot=$bcObjArr['isRoot'];
                $this->replace_if_exist=array_key_exists('replace_existing', $bcObjArr)?$bcObjArr['replace_existing']:false;

		$CI =& get_Instance();
		$CI->breadcrumblist->add($this);
		$_SESSION['breadCrumbList'] = $CI->breadcrumblist->getBreadCrumbs();
               
	}

	function getTitle(){
		return $this->title;
	}
	function setTitle($t){
		$this->title=$t;
	}
	function getUrl(){
		return $this->url;
	}
	function setUrl($u){
		$this->url=$u;
	}
	function isRoot(){
		return $this->isRoot;
	}
	function setIsRoot($r){
		$this->isRoot=$r;
	}

        function replaceExisting(){
            return $this->replace_if_exist;
        }
}
?>
