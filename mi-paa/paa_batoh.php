#!/usr/bin/php
<?php

define('VAHA', 0);
define('CENA', 1);
define('POMER', 2);

//function spocitej($filled, int * temp_arr, int new_index , double V_so_far, double C_so_far) {
function spocitej($predchozi_pole, $pridavany_prvek, $suma_vah, $suma_cen, $kapacita, $polozky) {
    if (($suma_vah + $polozky[$pridavany_prvek][VAHA])>$kapacita) {
        return null;
    }
    $ret = Array();
    $ret['CELKOVA_VAHA'] = $suma_vah + $polozky[$pridavany_prvek][VAHA];
    $ret['CELKOVA_CENA'] = $suma_cen + $polozky[$pridavany_prvek][CENA];
    $predchozi_pole[$pridavany_prvek] = 1;
    $ret['POLE'] = $predchozi_pole;
    
    $pocet = sizeof($polozky);
    $best_r = $ret;
    for ($i = $pridavany_prvek+1; $i < $pocet; $i++) {
        $r = spocitej($predchozi_pole, $i, $ret['CELKOVA_VAHA'], $ret['CELKOVA_CENA'], $kapacita, &$polozky);
        if ($r !== null && $r['CELKOVA_CENA'] > $best_r['CELKOVA_CENA']) $best_r = $r;
    }
    if ($best_r['CELKOVA_CENA']>$ret['CELKOVA_CENA']) $ret = $best_r;
    
    return $ret;
}

function cmp($a,$b) {
    return $a[POMER]-$b[POMER];
}


    if ($_SERVER['argc'] != 3) {
        echo "usage: " . $_SERVER['argv'][0] . " <inputfile> <outputfile>" ."\n" ;
        return 1;
    }
    $out = "";
    
    $file = file($_SERVER['argv'][1]);
    foreach ($file as $lines) {
        $reseni_brute = Array();
        $reseni_heuri = Array();
        $data = explode(" ", $lines);
        $id = array_shift($data);
        $pocet = array_shift($data);
        $kapacita = array_shift($data);
        $polozky = array();
        for ($i=0;$i<sizeof($data);$i=$i+2) {
            $item = Array();
            $item[VAHA] = $data[$i];
            $item[CENA] = $data[$i+1];
            $item[POMER] = $item[CENA]/$item[VAHA];
            $polozky[] = $item;
            $reseni_brute[] = 0;
            $reseni_heuri[] = 0;
        }
        
        
        
        $ret = Array();
        $ret['CELKOVA_VAHA'] = 0;
        $ret['CELKOVA_CENA'] = 0;
        $ret['POLE'] = $reseni_brute;
        $predchozi_pole = $reseni_brute;
        $best_r = $ret;
        for ($i = 0; $i < $pocet; $i++) {
            $r = spocitej($predchozi_pole, $i, $ret['CELKOVA_VAHA'], $ret['CELKOVA_CENA'], $kapacita, &$polozky);
            if ($r !== null && $r['CELKOVA_CENA'] > $best_r['CELKOVA_CENA']) $best_r = $r;
        }
        if ($best_r['CELKOVA_CENA']>$ret['CELKOVA_CENA']) $ret = $best_r;
        $out .= "$id $pocet " .$ret['CELKOVA_CENA']. " ";
        foreach($ret['POLE'] as $v)
            $out .= sprintf(" %d", $v);
        $out .= "\n";
        
        
        $serazene_polozky = $polozky;
//        print_r($serazene_polozky);
        usort($serazene_polozky, "cmp");
//        print_r($serazene_polozky);
        
        
        
        
        unset($serazene_polozky);
    }
    
    file_put_contents($_SERVER['argv'][2], $out);
    
