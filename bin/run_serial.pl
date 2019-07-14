$RESFILE = "./results/USA-road-d_serial.ss.res";
$PREFIX  = "./inputs/USA-road-d/USA-road-d";
$SOLVER  = "./SSSP_serial.exe";

$GRAPH   = "$PREFIX.%s.gr";
$AUX     = "$PREFIX.%s.ss";

print "\n* USA-road-d family ss core experiment\n";
print "* serial version\n";

open FILE, ">$RESFILE" or die "Cannot open $RESFILE for write :$!";
close FILE;

sub DORUN {
	$EXT = $_[0];
	$graphname = sprintf $GRAPH, $EXT;
	$auxname = sprintf $AUX, $EXT;

	print "\n Running serial ss solver on graph $graphname\n";
	$out = `$SOLVER $graphname $auxname $RESFILE 0 0`;
}

&DORUN("NY");
&DORUN("NE");
&DORUN("CTR");


$RESFILE = "./results/USA-road-t_serial.ss.res";
$PREFIX  = "./inputs/USA-road-t/USA-road-t";

$GRAPH   = "$PREFIX.%s.gr";
$AUX     = "$PREFIX.%s.ss";

print "\n* USA-road-t family ss core experiment\n";
print "* serial version\n";

open FILE, ">$RESFILE" or die "Cannot open $RESFILE for write :$!";
close FILE;

&DORUN("NY");
&DORUN("NE");
&DORUN("CTR");
