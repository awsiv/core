<?php

$hostkey = "SHA=265e5eb7abf2bb45a4e82b1753a95d047815f8ad2966d2cc8d205a027c50ba7d";
$vitalId = "time_watch";

// get list of vital ids

$ret = cfpr_vitals_list($hostkey);

echo "$ret\n";


// get magnified view of given host and vital id

$ret = cfpr_vitals_view_magnified($hostkey,$vitalId);

echo "MAG DATA\n---------------------\n$ret\n";


// get week view of given host and vital id

$ret = cfpr_vitals_view_week($hostkey,$vitalId);

echo "WEEK DATA\n-----------------------\n$ret\n";


// get year view of given host and vital id

$ret = cfpr_vitals_view_year($hostkey,$vitalId);

echo "YEAR DATA\n-----------------------\n$ret\n";


// get histograms of given host and vital id

$ret = cfpr_vitals_view_histogram($hostkey,$vitalId);

echo "HISTOGRAM DATA\n-----------------------\n$ret\n";


$ret = cfpr_vitals_analyse_magnified($hostkey,$vitalId);

echo "MAG ANALYSIS\n---------------------\n$ret\n";


$ret = cfpr_vitals_analyse_week($hostkey,$vitalId);

echo "WEEK ANALYSIS\n---------------------\n$ret\n";


$ret = cfpr_vitals_analyse_year($hostkey,$vitalId);

echo "YEAR ANALYSIS\n---------------------\n$ret\n";


$ret = cfpr_vitals_analyse_histogram($hostkey,$vitalId);

echo "HISTOGRAM ANALYSIS\n---------------------\n$ret\n";

?>
