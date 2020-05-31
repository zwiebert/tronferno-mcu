#!/usr/bin/perl

use strict;
use warnings;



while( my $line = <>)  {   
    print $line;    
    if ($line =~ /include:"(.+)"/) {
    	my $file_name = $1;
    	open(my $inc, $file_name) or die "error: cannot open file $file_name";
    	print <$inc>;
    	close($inc);
    }
}


