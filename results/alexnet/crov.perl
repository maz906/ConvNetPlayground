#!/usr/bin/perl
for (my $i = 1; $i <= 10; $i++) {
	$file = "crov_$i/caffe-train.log";
	open my $info, $file or die("Could not open file.");
	$outfile = "crov_$i/crov$i.csv";
	open(my $fh, '>', $outfile) or die "Could not open file '$outfile' $!";
	while (my $line = <$info>) 
	{
		if ($line =~ m/.*accuracy\s=\s(.*)/)
		{
			$accuracy = $1;
		}
		

		if ($line =~ m/.*?\d{1,3}000,\slr\s=\s(.*)/) 
		{
			$lr = $1;
		}

		if ($line =~ m/.*?(\d{1,3}000),\sloss\s=\s(.*)/) 
		{
			$iter_number = $1;
			$loss = $2;
			print $fh "$iter_number,$accuracy,$lr,$loss\n";
		}
	}
	close $fh; close $info;
}
