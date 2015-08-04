#!/usr/bin/perl

$file = "caffe-train.log";
open my $info, $file or die("Could not open file.");
$outfile = "google_val.csv";
open(my $fh, '>', $outfile) or die "Could not open file '$outfile' $!";
while (my $line = <$info>) 
{
	if ($line =~ m/.*Iteration\s(\d+),\sTesting\snet/)
	{
		$testing = 1;
		$iter = $1;
	}

	if ($testing && $line =~ m/.*Test\snet\soutput\s#8:\sloss3\/top-5\s=\s(.*)/)
	{
		$accuracy = $1;
		$testing = 0;
		$after_testing = 1;
	}
	

	if ($after_testing && $line =~ m/.*?\d{1,3}000,\sloss\s=\s(.*)/) 
	{
		$loss = $1;
	}

	if ($after_testing && $line =~ m/.*?(\d{1,3}000),\slr\s=\s(.*)/) 
	{
		$iter_number = $1;
		$lr = $2;
		$after_testing = 0;
		print $fh "$iter_number,$accuracy,$lr,$loss\n";
	}
}
close $fh; close $info;
