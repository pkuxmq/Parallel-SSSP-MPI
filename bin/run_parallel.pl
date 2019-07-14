$RESFILE = "./results/USA-road-d_parallel.ss.res";
$PREFIX  = "./inputs/USA-road-d/USA-road-d";
$SOLVER  = "./SSSP_parallel.exe";

$GRAPH   = "$PREFIX.%s.gr";
$AUX     = "$PREFIX.%s.ss";

print "\n* USA-road-d family ss core experiment\n";
print "* parallel version\n";

open FILE, ">$RESFILE" or die "Cannot open $RESFILE for write :$!";
close FILE;

sub DORUN {
	$EXT = $_[0];
	$graphname = sprintf $GRAPH, $EXT;
	$auxname = sprintf $AUX, $EXT;

	print "\n Running serial ss solver on graph $graphname\n";
	$core_num = 16;
	if ($EXT eq "NE") {
		$core_num = 16;
	}
	else {
		if ($EXT eq "CTR") {
			$core_num = 20;
		}
	}
	$out = `mpirun -np $core_num $SOLVER $graphname $auxname $RESFILE 0 0`;
}

&DORUN("NY");
&DORUN("NE");
&DORUN("CTR");

$RESFILE = "./results/USA-road-t_parallel.ss.res";
$PREFIX  = "./inputs/USA-road-t/USA-road-t";

open FILE, ">$RESFILE" or die "Cannot open $RESFILE for write :$!";
close FILE;

$GRAPH   = "$PREFIX.%s.gr";
$AUX     = "$PREFIX.%s.ss";

print "\n* USA-road-t family ss core experiment\n";
print "* parallel version\n";

&DORUN("NY");
&DORUN("NE");
&DORUN("CTR");
