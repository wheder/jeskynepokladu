<?php

$cnt_prvku_min = 10;
$cnt_prvku_max = 20;
$cnt_klauzuli_min = 5;
$cnt_klauzuli_max = 6;

//$hodnoty_prvku_min = 0;
//$hodnoty_prvku_max = 0;

$vahy_prvku_min = 1;
$vahy_prvku_max = 5;

$prvku = rand($cnt_prvku_min, $cnt_prvku_max);
$klauzuli = rand($cnt_klauzuli_min, $cnt_klauzuli_max);


echo $prvku . " ". $klauzuli  ." \n";
for ($i=0;$i<$prvku;$i++) echo rand($vahy_prvku_min, $vahy_prvku_max)." ";
echo "\n";


for ($i=0;$i<$klauzuli;$i++) {
	for ($j=0;$j<3;$j++) {
		if (rand(0,1)) echo "-";
		echo (rand(0,$prvku)+1). " ";
	}
	rand () . " ";
	echo "0 \n";

}





