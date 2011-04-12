<div id="placeholder" style="width:600px;height:300px;"></div> 



<?php
$array = json_decode($gdata);
var_dump($array);
$d1 = array();
$d2 = array();
$d3 = array();
foreach ($array as $index => $node) {
    $d2[] = array($index, $node[1]);
    $d3[] = array($index, $node[1] + $node[2]);
    $d1[] = array($index, $node[1] + $node[3]);
}

$kept = json_encode($d2);
$notkept = json_encode($d1);
$repaired = json_encode($d3);
?>



<script type="text/javascript">
    $(function () {
        var d1 = <?php echo $notkept ?>;
        var d2 = <?php echo $kept ?>;
        var d3 = <?php echo $repaired ?>;


        var dataset = [
            { id: 'repaired', data: d3, lines: { show: true, lineWidth: 1, fill: .4 }, color: "rgb(255,255,0)", fillBetween: 'kept' },
            { id: 'notkept', data: d1, lines: { show: true, lineWidth: 1, fill: .4}, color: "rgb(255,0,0)", fillBetween: 'kept' },
            { label: 'mean', data: d2,id:"kept", lines: { show: true,lineWidth:1 }, color: "rgb(0,255,0)" },
        ]

        $.plot($("#placeholder"), dataset);
    });
</script> 