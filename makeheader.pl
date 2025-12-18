#!/usr/bin/perl -w

use strict;

my ($basename, $midname, $endname, $infilename, $outfilename) = @ARGV;

my $perl = $ENV{PERL};

my $output_text = `$perl bin2c.pl THINGY < $infilename`;

$output_text =~ s/unsigned char THINGY\[\]/const unsigned char $basename\[\]/;

my $objlen = ($output_text =~ /unsigned int THINGY_len = (\d+)/)[0];

$output_text =~ s/unsigned int THINGY_len = (\d+)/const unsigned char \*$endname = $basename + $1/;

my $repltext = "const unsigned char \*$midname = $basename + $objlen - 4;";
$repltext = "" if ($midname eq '');

$output_text =~ s/int THINGY_mtime = \d+;/$repltext/;

open JELLY, ">", $outfilename;

print JELLY $output_text;

close JELLY;
