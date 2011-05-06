<?php

/**
 * Generates HTML table from JSON data structure for report generation
 */
class cf_table {

    var $dateTimeFields = array();
    var $ignoreDateColor = array();

    public function __construct() {
        $this->CI = &get_instance();
        $this->CI->load->library('table');

        // these array are the report titles where date should not be coloured 
        $this->ignoreDateColor = array('Promises not kept log','Promises repaired log');

        // these are the table fields that should be formatted as date and time
        $this->dateTimeField = array('lastseen', 'last seen', 'time', 'last verified', 'change detected at');
    }

    function generateReportTable($result,$report_title = '') {
        $return = '';
        if (is_array($result)) {
            $this->CI->table->set_heading(array_keys($result['meta']['header']));
            $heading = "";
            if (count($result['data']) > 0) {
                foreach ($result['data'] as $row) {
                    $temp = array();
                    foreach ($result['meta']['header'] as $key => $value) {
                        if (!is_array($value)) {
                            if (in_array(strtolower($key), $this->dateTimeField)) {
                                $color = in_array(trim($report_title),$this->ignoreDateColor);
                                array_push($temp, getDateStatus($row[$value],$color));
                            }
                            else{

                                $tempValue = $row[$value];
                                // make link to bundle and promises
                                if (strtolower($key) == 'promise handle') {
                                    $tempValue = sprintf('<a href="/promise/details/%s">%s</a>',urlencode($tempValue),$tempValue);
                                }
                                if (strtolower($key) == 'bundle') {
                                    $tempValue = sprintf('<a href="/bundle/details/bundle/%s">%s</a>',urlencode($tempValue),$tempValue);
                                }

                                array_push($temp, $tempValue);
                            }

                        }
                        else {
                            if (strtolower($key) == "note") {
                                $link = site_url("notes") . '/index/';
                                $data_index = $value['index'];
                                // var_dump($row[$data_index] );
                                foreach ($value['subkeys'] as $subkey => $subval) {
                                    $data = trim($row[$data_index][$subval]);
                                    if ($subkey == 'rid') {
                                        $data = urlencode(base64_encode(utf8_encode($data)));
                                    }
                                    if ($data != '') {
                                        $link.="$subkey/$data/";
                                    }
                                }
                                array_push($temp, anchor($link, 'note', 'class=note'));
                            }
                        }
                    }
                    $this->CI->table->add_row($temp);
                }
            }
            $return = $this->CI->table->generate();
        }
        return  $return;
    }

    function generate_nested_table($result,$title='')
    {
        foreach ($result['data'] as $row)
       {
        //$this->table->add_row($row);
           $temp=array();
           $this->CI->table->set_heading(array_keys($result['meta']['header']));
           $heading="";
           foreach($result['meta']['header'] as $key=>$value)
           {
              if(!is_array($value))
              {
                 if(in_array(strtolower($key), $this->dateTimeField))
                      array_push($temp,  date('D F d h:m:s Y',$row[$value]));
                  else if(strtolower($key)=="")
                  {
                     $content="<span class=\"$row[$value]host coloricon\"></span>";
                     array_push($temp, $content);
                  }
                  else if(strtolower($key) == 'service bundle name')
                  {
                   $content=sprintf('<a href="/bundle/details/bundle/%s/type/%s">%s</a>',urlencode($row[$value]),$row[$result['meta']['header']['Type']],$row[$value]);
                     array_push($temp, $content);
                  }
                  else
                  {
                    array_push($temp, $row[$value]);
                  }
              }
             else
             {
                    if(is_array($row[$value['index']]))
                    {
                    $table="<table><tr>";
                    $goal_link="";
                          $subindexs=  array_values($value['subkeys']);
                          $index=$value['index'];
                          if($row[$index][$value['subkeys']['name']]=="Unknown")
                          {
                              $cell="unknown";
                              $table.="<td>$cell</td>";
                          }
                          else
                          {
                               foreach($subindexs as $subindex)
                                {
                                 if($subindex ==0)continue;
                                //pid=0 name=1 and description=2
                                 $cell=$row[$index][$subindex];
                                 if($subindex !=2 )
                                    {
                                     $cell="<a href=".site_url ("knowlege/pid/").$row[$index][0].">".$row[$index][1]."</a>";
                                    }
                                  $table.="<td>$cell</td>";
                                }
                          }     
                      $table.="</tr></table>";
                      array_push($temp, $table);
                    }
              }
           }
          $this->CI->table->add_row($temp);
       }
      return $this->CI->table->generate();
    }

}

?>
