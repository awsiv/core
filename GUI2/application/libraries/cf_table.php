<?php

/**
 * Generates HTML table from JSON data structure for report generation
 */
class cf_table {

    var $dateTimeFields = array();
    var $ignoreDateColor = array();
     var $template = array(
            'table_open' => '<table border="0" style="width:100%;" cellpadding="0" cellspacing="0">',
            'thead_open' => '<thead>',
            'thead_close' => '</thead>',
            'heading_row_start' => '<tr>',
            'heading_row_end' => '</tr>',
            'heading_cell_start' => '<th>',
            'heading_cell_end' => '</th>',
            'tbody_open' => '<tbody>',
            'tbody_close' => '</tbody>',
            'row_start' => '<tr>',
            'row_end' => '</tr>',
            'cell_start' => '<td>',
            'cell_end' => '</td>',
            'row_alt_start' => '<tr>',
            'row_alt_end' => '</tr>',
            'cell_alt_start' => '<td>',
            'cell_alt_end' => '</td>',
            'table_close' => '</table>'
        );

    public function __construct() {
        $this->CI = &get_instance();
        $this->CI->load->library('table');

        // these array are the report titles where date should not be coloured
        $this->ignoreDateColor = array('Promises not kept log',
            'Promises repaired log',
            'Compliance summary',
            'File change log',
            'File change diffs',
            'Performance');

        // these are the table fields that should be formatted as date and time
        $this->dateTimeField = array('lastseen', 'last seen', 'time', 'last verified', 'change detected at', 'last performed', 'last occurred', 'was last seen at');
    }

    function generateHeadings($header)
    {

        $heading = '<tr>';
        foreach ($header as $title)
        {
            $heading .= '<th>' . $title . '</th>';
        }
        $heading .= '</tr>';
        return $heading;
    }

    function generateRows($row)
    {
        if (!is_array($row)) {
            $rowArray['data'] = $row;
        } else {
            $rowArray = $row;
        }

        $tr = '<tr>';
        $attributes = '';
        foreach ($rowArray as $td)
        {
            if (is_array($td) && array_key_exists('data', $td)) {
                $value = $td['data'];
               foreach ($td as $key=>$val) {
                   if ($key != 'data') {
                   $attributes .= sprintf('%s="%s" ',$key,$val);
                   }
               }
            } else {
                $value = $td;
            }

            $tr .= sprintf('<td %s>%s</td>',$attributes,  $value);
        }
        $tr .= '</tr>';
        return $tr;
    }

    /**
     * Optimized for less memory usage, doesnt uses CI table generator
     * @param type $result
     * @param type $report_title
     * @return type
     */
    function generateReportTableModified($result, $report_title = '')
    {
        $return = '';
        $this->CI->table->clear();

        if (is_array($result))
        {
            $tableHeader = $this->generateHeadings(array_keys($result['meta']['header']));
            $tableBody = '';
            $heading = "";
            if (count($result['data']) > 0)
            {
                foreach ($result['data'] as $row)
                {
                    $temp = array();
                    foreach ($result['meta']['header'] as $key => $value)
                    {
                        if (!is_array($value))
                        {
                            if (in_array(strtolower($key), $this->dateTimeField))
                            {
                                $color = in_array(trim($report_title), $this->ignoreDateColor);
                                array_push($temp, getDateStatus($row[$value], $color));
                            }
                            else
                            {

                                $tempValue = $row[$value];
                                // make link to bundle and promises
                                if (strtolower($key) == 'promise handle')
                                {
                                    $tempValue = sprintf('<a target="_self" href="%s/promise/details/%s">%s</a>', site_url(), urlencode($tempValue), $tempValue);
                                }
                                else if (strtolower($key) == 'bundle')
                                {
                                    $tempValue = sprintf('<a target="_self" href="%s/bundle/details/bundle/%s">%s</a>', site_url(), urlencode($tempValue), $tempValue);
                                }
                                else if (strtolower($key) == 'virtual bundle')
                                {
                                    $tempValue = sprintf('<a target="_self" href="%s/virtualbundle/details/%s">%s</a>', site_url(), trim(urlencode($tempValue)), $tempValue);
                                }


                                array_push($temp, $tempValue);
                            }
                        }
                        else
                        {
                            if (strtolower($key) == "note")
                            {
                                $link = site_url("notes") . '/index/';
                                $data_index = $value['index'];
                                $notesAlreadyPresent = false;
                                foreach ($value['subkeys'] as $subkey => $subval)
                                {
                                    $data = trim($row[$data_index][$subval]);



                                    if ($subkey == 'rid')
                                    {
                                        $data = urlencode((utf8_encode($data)));
                                    }
                                    if ($data != '')
                                    {
                                        $link.="$subkey/$data/";
                                        // if subval action is show we can indicate it visually there are some comments
                                        if ($subkey == 'action' && $data == 'show')
                                            $notesAlreadyPresent = true;
                                    }
                                }
                                $addClass = $notesAlreadyPresent ? 'notes-highlight' : '';
                                $c = array(
                                    'data' => anchor($link, 'notes', array('class' => 'note')),
                                    'class' => $addClass);
                                array_push($temp, $c);
                            }
                        }
                    }
                    //$this->CI->table->add_row($temp);
                    $tableBody .= $this->generateRows($temp);

                }
            }
            $return = sprintf('<table><thead>%s</thead><tbody>%s</tbody></table>',$tableHeader,$tableBody);
        }
        return $return;
    }

    function generateReportTable($result, $report_title = '')
    {
        $return = '';
        $this->CI->table->clear();

        $this->CI->table->set_template($this->template);


        if (is_array($result)) {
            $this->CI->table->set_heading(array_keys($result['meta']['header']));
            $heading = "";
            if (count($result['data']) > 0) {
                foreach ($result['data'] as $row) {
                    $temp = array();
                    foreach ($result['meta']['header'] as $key => $value) {
                        if (!is_array($value)) {
                            if (in_array(strtolower($key), $this->dateTimeField)) {
                                $color = in_array(trim($report_title), $this->ignoreDateColor);
                                array_push($temp, getDateStatus($row[$value], $color));
                            } else {

                                $tempValue = $row[$value];
                                // make link to bundle and promises
                                if (strtolower($key) == 'promise handle') {
                                    $tempValue = sprintf('<a target="_self" href="%s/promise/details/%s">%s</a>', site_url(), urlencode($tempValue), $tempValue);
                                }
                                else if (strtolower($key) == 'bundle') {
                                    $tempValue = sprintf('<a target="_self" href="%s/bundle/details/bundle/%s">%s</a>', site_url(), urlencode($tempValue), $tempValue);
                                }
                                 else if (strtolower($key) == 'virtual bundle') {
                                    $tempValue = sprintf('<a target="_self" href="%s/virtualbundle/details/%s">%s</a>', site_url(), trim(urlencode($tempValue)), $tempValue);
                                }


                                array_push($temp, $tempValue);
                            }
                        } 
                    }
                    $this->CI->table->add_row($temp);
                }
            }
            $return = $this->CI->table->generate();
        }
        return $return;
    }




    function generate_nested_table($result, $title='') {
        foreach ((array) $result['data'] as $row) {
            //$this->table->add_row($row);
            $temp = array();
            $this->CI->table->set_heading(array_keys($result['meta']['header']));
            $heading = "";
            foreach ($result['meta']['header'] as $key => $value) {
                if (!is_array($value)) {
                    if (in_array(strtolower($key), $this->dateTimeField))
                        array_push($temp, date('D F d h:m:s Y', $row[$value]));
                    else if (strtolower($key) == "") {
                        $content = "<span class=\"$row[$value]host coloricon\"></span>";
                        array_push($temp, $content);
                    } else if (strtolower($key) == 'service bundle name') {
                        $content = sprintf('<a href="%s/bundle/details/bundle/%s/type/%s">%s</a>', site_url(), urlencode($row[$value]), $row[$result['meta']['header']['Type']], $row[$value]);
                        array_push($temp, $content);
                    } else {
                        array_push($temp, $row[$value]);
                    }
                } else {
                    if (is_array($row[$value['index']])) {
                        $table = "<table><tr>";
                        $goal_link = "";
                        $subindexs = array_values($value['subkeys']);
                        $index = $value['index'];
                        /* if($row[$index][$value['subkeys']['name']]=="Unknown")
                          {
                          $cell="unknown";
                          $table.="<td>$cell</td>";
                          }
                          else
                          { */
                        if (count($row[$index]) > 0) {
                            foreach ($row[$index] as $secodtablerow) {
                                $table.="<tr>";
                                foreach ($subindexs as $subindex) {
                                    if ($subindex == 0)
                                        continue;
                                    //pid=0 name=1 and description=2
                                    if (strcmp(strtolower($secodtablerow[$subindex]), "unknown") == 0) {
                                        continue;
                                    }
                                    $cell = $secodtablerow[$subindex];
                                    if ($subindex != 2) {
                                        $cell = "<a href=" . site_url("/knowledge/knowledgemap/pid") . "/" . $secodtablerow[0] . ">" . $secodtablerow[1] . "</a>";
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

    function generateSingleColourHostTable($data, $type) {

        $lnk = "";
        if ($type == 'blue') {
            $this->CI->table->set_heading('hostname', 'last data', 'Action');
        } else if ($type == 'black'){
            $this->CI->table->set_heading('hostname', 'last agent execution', 'Action');
        } else {
            $this->CI->table->set_heading('hostname', 'Action');
        }
        foreach ((array) $data as $cols) {
            //array_push($columns, img(array('src' => 'images/' . $type . '.png', 'class' => 'align')) . anchor('welcome/host/' . $cols['key'], $cols['id'], 'class="imglabel"'));
            $title = $cols['id'];
            if ($cols['id'] == "") {
                $title = $cols['key'];
            }
            if ($type == 'blue') {
                $lnk = isset($cols['lastseen'])?getDateStatus($cols['lastseen'],true):"never";
                $cell2 = array('data' => $lnk, 'class' => 'datecol');
                $btns = anchor('visual/vital/' . $cols['key'], ' ', array('title' => $this->CI->lang->line('tooltip_vital_signs'), 'class' => 'vitalsbtn showqtip'))
                        . anchor('welcome/host/delhost/' . $cols['key'] . '/type/' . $type, ' ', array('title' => 'delete host', 'class' => 'deletehostbtn showqtip'));
                $cell3 = array('data' => $btns, 'class' => 'actioncol');
                $delChkBox=array('name'=>'host[]','value'=>$cols['key'],'style'=>'margin-right:9px');
                $this->CI->table->add_row(array(
                    form_checkbox($delChkBox).anchor('welcome/host/' . $cols['key'], $title, 'class="imglabel"'),
                    $cell2,
                    $cell3
                ));
            } else if ($type == 'black') {
                $lnk = isset($cols['lastexec'])?getDateStatus($cols['lastexec'],true):"never";
                $cell2 = array('data' => $lnk, 'class' => 'datecol');
                $btns = anchor('visual/vital/' . $cols['key'], ' ', array('title' => $this->CI->lang->line('tooltip_vital_signs'), 'class' => 'vitalsbtn showqtip'))
                        . anchor('welcome/host/delhost/' . $cols['key'] . '/type/' . $type, ' ', array('title' => 'delete host', 'class' => 'deletehostbtn showqtip'));
                $cell3 = array('data' => $btns, 'class' => 'actioncol');
                $delChkBox=array('name'=>'host[]','value'=>$cols['key'],'style'=>'margin-right:9px');
                $this->CI->table->add_row(array(
                    form_checkbox($delChkBox).anchor('welcome/host/' . $cols['key'], $title, 'class="imglabel"'),
                    $cell2,
                    $cell3
                ));
            } else {
                $lnk = anchor('search/index/report/promises-not-kept-summary/host/' . $cols['key'], ' ', array('title' => 'promises not kept', 'class' => 'promisesnotkeptbtn showqtip'))
                        . anchor('visual/vital/' . $cols['key'], ' ', array('title' => 'pulse and vitals', 'class' => 'vitalsbtn showqtip'))
                        . anchor('welcome/host/delhost/' . $cols['key'] . '/type/' . $type, ' ', array('title'=>tooltip('tool_tip_host_delete'), 'class' => 'deletehostbtn showqtip'));
                $cell = array('data' => $lnk, 'class' => 'actioncol');
                $delChkBox=array('name'=>'host[]','value'=>$cols['key'],'style'=>'margin-right:9px');
                $this->CI->table->add_row(array(
                    form_checkbox($delChkBox).anchor('welcome/host/' . $cols['key'], $title, 'class=""'),
                    $cell,
                ));
            }
        }
        return $this->CI->table->generate();
    }

    function generate_cdp_report_table($report, $tabledata) {
        if (!is_array($tabledata)) {
            return "invalid json data";
        }
        $escape_header = array('hostkey', 'urlReport', 'timewarn');
        $links_only_for = array('file diffs' => 'Path');
        $time_fields = array('Last checked');
        $headers = array();
        foreach ($tabledata['meta']['header'] as $header => $value) {
            if (array_search($header, $escape_header) === FALSE) {
                array_push($headers, $header);
            }
        }

        $this->CI->table->set_heading($headers);
        if (count($tabledata['data']) > 0) {
            foreach ($tabledata['data'] as $row) {
                $temp = array();
                foreach ($tabledata['meta']['header'] as $key => $value) {
                    if (array_search($key, $escape_header) === FALSE) {
                        if (key_exists(strtolower($report), $links_only_for) && $links_only_for[strtolower($report)] == $key) {
                            $content = sprintf('<a href="%s/search/index/report/%s/host/%s/name/%s">%s</a>'
                                    , site_url(), urlencode($row[$tabledata['meta']['header']['urlReport']]), urlencode($row[$tabledata['meta']['header']['hostkey']]), urlencode($row[$value]), $row[$value]);
                            array_push($temp, $content);
                        } elseif (in_array($key, $time_fields)) {
                            $content = "";
                            if ($row[$tabledata['meta']['header']['timewarn']]) {
                                $content = "<span class=\"amber\">" . $row[$value] . " </span>";
                                array_push($temp, $content);
                            } else {
                                array_push($temp, $row[$value]);
                            }
                        } else {
                            array_push($temp, $row[$value]);
                        }
                    }
                }
                $this->CI->table->add_row($temp);
            }
            return $this->CI->table->generate();
        }
    }

    function generateVirtualbundleTable($result){
        $return = '';
        $this->CI->table->clear();
        $heading=$result['meta']['header'];
        $heading['']=count($result['meta']['header']);
        $this->CI->table->set_template($this->template);
        if (is_array($result)) {
            $this->CI->table->set_heading(array_keys($heading));
            $heading = "";
            if (count($result['data']) > 0) {
                foreach ($result['data'] as $row) {
                    $temp = array();
                    $handle='';
                    foreach ($result['meta']['header'] as $key => $value) {
                        if (!is_array($value)) {
                                if(strtolower($key)=='virtual bundle'){
                                    $handle=$row[$value];
                                    $tempValue=anchor('virtualbundle/details/'.urlencode($handle),$handle);
                                }
                                else{
                                  $tempValue = $row[$value];
                                }
                                array_push($temp, $tempValue);
                        }
                    }
                    $actionlink=anchor('virtualbundle/edit/'.urlencode($handle), ' ', array('class'=>'edit')).
                                    anchor('virtualbundle/delete/handle/'.urlencode($handle), ' ', array('class'=>'delete'));
                     array_push($temp, $actionlink);
                    $this->CI->table->add_row($temp);
                }
            }
            $return = $this->CI->table->generate();
        }
        return $return;
    }

  function host_only_table($data) {
   $this->CI->table->set_heading('hostname', 'Action');

        foreach ((array) $data['data'] as $rows)
        {
            $lnk = anchor('search/index/report/promises-not-kept-summary/host/' . $rows['hostkey'], ' ', array('title' => 'promises not kept', 'class' => 'promisesnotkeptbtn showqtip'))
                    . anchor('visual/vital/' . $rows['hostkey'], ' ', array('title' => 'pulse and vitals', 'class' => 'vitalsbtn showqtip'));

            $cell = array('data' => $lnk, 'class' => 'actioncol');
            $this->CI->table->add_row(array(
                anchor('welcome/host/' . $rows['hostkey'], $rows['hostname'], 'class=""'),
                $cell,
            ));
        }
        return $this->CI->table->generate();
     }

}

?>
