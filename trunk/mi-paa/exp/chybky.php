<?php

if ($_SERVER['argc'] != 3 ) die("USAGE: ".$_SERVER['argv'][0]." EXACT HEUR\n");

$max_err = 0.0;

$exact = file($_SERVER['argv'][1]);
$heuri = file($_SERVER['argv'][2]);

// ε = ( C(OPT)-C(APX) ) / C(OPT)
$chyby = array();
for ($i = 0; $i < sizeof($exact); ++$i) {
	$ex_arr = explode(" ", $exact[$i]);
	$he_arr = explode(" ", $heuri[$i]);
	$chyby[] = ( ($ex_arr[1]-$he_arr[1]) / $ex_arr[1]  );
}


echo $_SERVER['argv'][2] ." ".(array_sum($chyby)/sizeof($chyby))." ".max($chyby)."\n";


