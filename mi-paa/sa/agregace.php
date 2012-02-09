<?php
//$max_err = 0.0;

//$exact = file($_SERVER['argv'][1]);
//$heuri = file($_SERVER['argv'][2]);

// ε = ( C(OPT)-C(APX) ) / C(OPT)
//$chyby = array();


ini_set("precision", 18);

$res = file("moc", FILE_IGNORE_NEW_LINES);
$petice = Array();
foreach ($res as $nr => $line) {
	$ex = explode(" ", $line);
	if ($nr%5 == 0 && $nr !==0) {
		$time = 0.0; $chyba = 0.0;
		foreach ($petice as $item) {
			$time += (double)$ex[3];
			$chyba += (double)$ex[8];
		}
		echo $petice[0][0]." ".$petice[0][1]." ".$petice[0][2]." ".sprintf("%.18f",($time/5))." ".$petice[0][4]." ".$petice[0][5]." ".$petice[0][6]." ".$petice[0][7]." ".sprintf("%.10f",($chyba/5))."\n";
		
		$petice = Array();
	}
	
	$petice[] = $ex;
	
	
	
}



		$time = 0.0; $chyba = 0.0;
		foreach ($petice as $item) {
			$time += (double)$ex[3];
			$chyba += (double)$ex[8];
		}
		echo $petice[0][0]." ".$petice[0][1]." ".$petice[0][2]." ".sprintf("%.18f",($time/5))." ".$petice[0][4]." ".$petice[0][5]." ".$petice[0][6]." ".$petice[0][7]." ".sprintf("%.10f",($chyba/5))."\n";





