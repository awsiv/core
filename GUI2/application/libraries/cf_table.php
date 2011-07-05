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
        $this->dateTimeField = array('lastseen', 'last seen', 'time', 'last verified', 'change detected at','last performed');
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
                                    $tempValue = sprintf('<a target="_self" href="/promise/details/%s">%s</a>',urlencode($tempValue),$tempValue);
                                }
                                if (strtolower($key) == 'bundle') {
                                    $tempValue = sprintf('<a target="_self" href="/bundle/details/bundle/%s">%s</a>',urlencode($tempValue),$tempValue);
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
                                        $data = urlencode((utf8_encode($data)));
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
        foreach ((array)$result['data'] as $row)
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
                        /* if($row[$index][$value['subkeys']['name']]=="Unknown")
                          {
                              $cell="unknown";
                              $table.="<td>$cell</td>";
                          }
                          else
                          {*/
                              if(count($row[$index])>0)
                              {
                                  foreach($row[$index] as $secodtablerow)
                                  {
                                      $table.="<tr>";
                                      foreach($subindexs as $subindex)
                                        {
                                         if($subindex ==0)continue;
                                        //pid=0 name=1 and description=2
                                          if(strcmp(strtolower($secodtablerow[$subindex]), "unknown")==0)
                                              {
                                                  continue;
                                              }
                                         $cell=$secodtablerow[$subindex];
                                         if($subindex !=2 )
                                            {
                                             $cell="<a href=".site_url ("/knowledge/knowledgemap/pid")."/".$secodtablerow[0].">".$secodtablerow[1]."</a>";
                                            }
                                            $table.="<td>$cell</td>";
                                        }
                                         $table.="</tr>";
                                  }
                              }
                        //  }
                      $table.="</tr></table>";
                      array_push($temp, $table);
                    }
              }
           }
          $this->CI->table->add_row($temp);
       }
      return $this->CI->table->generate();
    }

    function generateSingleColourHostTable($data,$type) {

       $lnk="";
       if($type=='blue'){
         $this->CI->table->set_heading('hostname','last seen','vitals');
       }else{
          $this->CI->table->set_heading('hostname','Action');
       }
 foreach ($data as $cols) {
                //array_push($columns, img(array('src' => 'images/' . $type . '.png', 'class' => 'align')) . anchor('welcome/host/' . $cols['key'], $cols['id'], 'class="imglabel"'));
       if($type=='blue'){
         $lnk=date('D F d h:m:s Y',$cols['lastseen']);
         $cell2=array('data' => $lnk, 'class' => 'datecol');
         $cell3=array('data' => anchor('visual/vital/' . $cols['key'], ' ', array('title' => 'pulse and vitals','class'=>'vitalsbtn')), 'class' => 'actioncol');
         $this->CI->table->add_row(array(
                  anchor('welcome/host/' . $cols['key'], $cols['id'], 'class="imglabel"'),
                  $cell2,
                  $cell3
                  ));
       }else{
          $lnk=anchor('search/index/report/Promises+not+kept+summary/host/'.$cols['key'],' ',array('title' => 'promises not kept','class'=>'promisesnotkeptbtn'))
               .anchor('visual/vital/' . $cols['key'], ' ', array('title' => 'pulse and vitals','class'=>'vitalsbtn'));
          $cell = array('data' => $lnk, 'class' => 'actioncol');
          $this->CI->table->add_row(array(
                  anchor('welcome/host/' . $cols['key'], $cols['id'], 'class=""'),
                  $cell,
                  ));
          
       }
         }
     return $this->CI->table->generate();
    }
}

?>
