<?php
//接收发来的电压和模式信息，并将合适的字符串存入data中等待发送，我实在懒得学数据库了。。
//Ω
	$volt=(float)$_GET['content'];
	$mode=(int)$_GET['mode']-48;
	$output=date('[Y-m-d H:i:s]',time())." 测得";
	if($mode==0)$output.="电压 ";else $output.="电阻 ";
	if($mode==1)$rx=$volt*220/(5-$volt);
	if($mode==2)$rx=$volt*1000/(5-$volt);
	if($mode==3)$rx=$volt*10000/(5-$volt);
	$rx=(int)$rx;
	$datafile=fopen("data","w");
	if($mode==0)$output.=(string)$volt;else $output.=(string)$rx;
	if($mode==0)$output.="V";else $output.="Ω";
	fwrite($datafile,$output);
	fclose($datafile);
?>
