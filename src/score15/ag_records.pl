#!/usr/bin/perl
#
# ag_records.pl: read through all results files for the noontime
# competition and publish (or update) list of age_group records
# Author: Terry R. McConnell (trmcconn@syr.edu)

# Packages to import
use Getopt::Std;

# Global variables and flags
$version = ".05";
$verbosity = 1;    # increase to 2 or 3 for more garbage
$use_input_format = 0;
$open_ag = 0;
$new_mode = 0;

# Subroutines
sub is_better;
sub lookup_age;
sub parse_date;

# the default place where all roster and results files are stored
$working_dir = "$ENV{HOME}/src/score15";

# the roster files to process

@rosters = qw(
	roster.rec
	roster.04
	roster.05
	roster.asc
);

# The results files to process

@results = qw(
	results.rec
	results.04
	results.05
	results.asc
);

# The main records database. A hash of hashes of arrays.

# This will be grown during the read of the results files.
# What's here is nonsense, just to show the format.
@bests = {
		"400m" => {
			"m29" => {
					date => "1/1/1990",
					name => "tmcconnell",
					time => "2:00",
			},
			"f29" => {
					date => "1/1/1990",
					name => "pford",
					time => "3:00",
			},
		},
};

$verbosity >= 1 and print <<BANNER;
		Age Group Records Rubbish Lister Version $version
			    by Terry R. McConnell
		            Perl version $]


BANNER

$USAGE = "Usage: ag_records.pl [-ioqnh] [-d <dir> -v <level>]\n";

# Process command line options

getopts('hioqnd:v:');
if($opt_h){
	print <<HELP;
$USAGE
-h: Prints this helpful information.
-q: Quiet. Same as -v 0.
-o: Generate open records rather than age group records.
-i: Update record results and roster files. 
-d: Use the following argument as the working directory.
-n: Display records not in results.rec, i.e, new ones.
-v: Use the following argument as the verbosity level.
HELP
	exit 0;
}
if($opt_i){
	$use_input_format = 1;
}
if($opt_q){
	$verbosity = 0;
}
if($opt_v){
	$verbosity = $opt_v;
}
if($opt_o){
	$open_ag = 1;
}
if($opt_d){
	$working_dir = $opt_d;
}
if($opt_n){
	$new_mode = 1;
}

chdir $working_dir or die "Cannot cd to $working_dir\n";

# Process roster files: read them all and form a hash indexed by runners
# names with their sex and DOB

foreach $roster (@rosters) {

	$verbosity >= 1 and print "Processing $roster ...\n";
	open CURRENT_INPUT, $roster or die "Cannot open $roster\n";

	$line_cnt = 0;
	$males = 0;
	$females = 0;
	while($line = <CURRENT_INPUT>){
		next if $line =~ /^\*/;  # Skip lines beginning with *
		next if $line =~ /^\s*$/; # Skip empty lines
		$line_cnt++;

		# parse line and add to runners hash
		($name,$mo,$day,$year,$sex,$team) = split /\s+/, $line;
		$runners{$name} = [ $mo,$day,$year,$sex];
		if($sex eq "m"){ $males++; }	
		if($sex eq "f"){ $females++;}
	}
	$verbosity == 2 and print "Data lines = $line_cnt, $males males, $females females\n";
	close CURRENT_INPUT;
}

$roster_total = keys %runners;
$verbosity == 2 and print "Combined roster size = $roster_total\n";

# Process all of the results file. Each file is processed in blocks 
# delineated by lines of the form
# date [datestring] [type]
# distance [event]

# In between are lines of the form
# name timestring
# 
$file_count = 0;
foreach $result_file (@results)
{
	$verbosity >= 1 and print "Processing $result_file ...\n";
	open CURRENT_INPUT, $result_file or die "Cannot open $result_file\n";
	
	$file_count++;
	$regular_count = 0;
	$outside_count = 0;
	$informal_count = 0;
	$event_count = 0;
	$current_date = "NA";
	$current_event = "NA";
	$performance_count = 0;
	$ag_count = 0;
	$event_count = 0;

	while($line = <CURRENT_INPUT>){
		next if $line =~ /^\*/;  # Skip lines beginning with *
		next if $line =~ /^\s*$/; # Skip empty lines


	# What type of line is it? Look at first field to find out

		if ($line =~ /^\s*date\s+([^ 	]+)\s+regular/) {
			$current_date = $1;
			$regular_count++;
			next;
		}
		elsif ($line =~ /^\s*date\s+([^ 	]+)\s+outside/) {
			$current_date = "NA";
			$outside_count++;
			next;
		}
		elsif ($line =~ /^\s*date\s+([^ 	]+)\s+informal/) {
			$current_date = "NA";
			$informal_count++;
			next;
		}
		elsif ($line =~ /^\s*distance\s+([^ 	]+)\s*/) {
			$current_event = $1;
			$event_count++;
			next;
		}
		else {  # Here's the guts. We must look up the current
                        # event in the bests hash, creating an entry if
                        # it isn't there. 

			# line should have format
			# name time

			# first skip over things in an outside or informal
                        # block

			next if $current_date eq "NA";

			# also skip if the current event is a relay
			
			next if $current_event =~ /.*r/;

			# or if it is the ill-fated XC event

			next if $current_event =~ /XC/;


			$_ = $line;
			m/^\s*([^ 	]+)\s+([^ 	]+)\s*/;
			$current_time = $2;
			$current_name = $1; 

			# Also skip if person is not in roster
			if(!$runners{$current_name}){next;}

			$current_ag = lookup_age $current_name, $current_date;

			# If there is no entry for current age group and
                        # and event, create  one with a ridiculously large
                        # time
			if(!$bests{$current_event}){
				$bests{$current_event}->{$current_ag}->{"time"}
					= "60:00:00";
				$bests{$current_event}->{$current_ag}->{"name"}
					= "N/A";
				$bests{$current_event}->{$current_ag}->{"date"}
					= "N/A";
			}
			if(!$bests{$current_event}->{$current_ag}){
				$bests{$current_event}->{$current_ag}->{"time"}
					= "60:00:00";
				$bests{$current_event}->{$current_ag}->{"name"}
					= "N/A";
				$bests{$current_event}->{$current_ag}->{"date"}
					= "N/A";
			}
			if(is_better $current_time,
			     $bests{$current_event}->{$current_ag}->{"time"}){
				if($new_mode && ($file_count == 2)){
				 print "New Record! $current_event, $current_ag:\n";
				 print "$current_time $current_name $current_date\n";
				 print "\n\t*** beats ***\n\n";
				   $old_time = $bests{$current_event}->{$current_ag}->{"time"};
				   $old_name = $bests{$current_event}->{$current_ag}->{"name"};
				   $old_date = $bests{$current_event}->{$current_ag}->{"date"};
				 print "$old_time $old_name $old_date\n\n";
				}
				$bests{$current_event}->{$current_ag}->{"time"}	= $current_time;
				$bests{$current_event}->{$current_ag}->{"name"}	= $current_name;
				$bests{$current_event}->{$current_ag}->{"date"}	= $current_date;
				$verbosity == 3 and 
				 print "Record $current_event, $current_ag:\n";
				$verbosity == 3 and
				 print "\t$current_date $current_name $current_time\n";
			}
			$performance_count++;
		}
	
	}
	$verbosity == 3 and print "$regular_count regulars, $outside_count outsides, $informal_count informals, $event_count events, $performance_count performanes\n";
	close CURRENT_INPUT;
}

exit 0 if $new_mode;

# printout the results
print "\n";

if($use_input_format){
	open OUTPUT, '>results.rec' or die "Cannot open results.rec for writing\n";
	select OUTPUT;
	print "*This file was generated by ag_records.pl version $version\n";
	print "*It holds record setting results in the format needed by\n";
	print "*the ag_records.pl program\n\n";
  	foreach $event_name (sort keys %bests){
	  foreach $age_group (sort keys %{$bests{$event_name}}){
		$the_time = $bests{$event_name}->{$age_group}->{"time"};
		$the_name = $bests{$event_name}->{$age_group}->{"name"};
		$the_date =  $bests{$event_name}->{$age_group}->{"date"};
		print "date $the_date regular\n";
		print "distance $event_name\n";
		print "$the_name $the_time\n\n";
	  }
	}
	close OUTPUT;
	open OUTPUT, '>roster.rec' or die "Cannot open roster.rec for writing\n";
	select OUTPUT;
	print "*This file was generated by ag_records.pl version $version\n";
	print "*It holds a roster to be used in conjuction with results.rec\n";
	print "*by the ag_records.pl program.\n\n";
	foreach $name (sort keys %runners){
		$mo = $runners{$name}[0];
		$day = $runners{$name}[1];
		$yr = $runners{$name}[2];
		$sex = $runners{$name}[3];
		print "$name $mo $day $yr $sex una\n";
	}
	close OUTPUT;
	exit 0;
}

if($open_ag){
	print "Open records\n";
	print "==============================================================\n";
  	foreach $event_name (sort keys %bests){
		$the_time = $bests{$event_name}->{"f29"}->{"time"};
		$the_name = $bests{$event_name}->{"f29"}->{"name"};
		$the_date =  $bests{$event_name}->{"f29"}->{"date"};
		printf "Women's %6s: %-12s%-20s %10s\n",
		  $event_name, $the_time,$the_name, $the_date;
	}
  	foreach $event_name (sort keys %bests){
		$the_time = $bests{$event_name}->{"m29"}->{"time"};
		$the_name = $bests{$event_name}->{"m29"}->{"name"};
		$the_date =  $bests{$event_name}->{"m29"}->{"date"};
		printf "Men's   %6s: %-12s%-20s %10s\n",
		  $event_name, $the_time,$the_name, $the_date;
	}
}
else {
  foreach $event_name (sort keys %bests){
	print "Age group records for $event_name\n";
	print "==============================================================\n";
	foreach $age_group (sort keys %{$bests{$event_name}}){
		$the_time = $bests{$event_name}->{$age_group}->{"time"};
		$the_name = $bests{$event_name}->{$age_group}->{"name"};
		$the_date =  $bests{$event_name}->{$age_group}->{"date"};
		printf "%s: %-12s%-20s %10s\n",
		  $age_group, $the_time,$the_name, $the_date;
	}
	print "\n\n";
  }
}

# usage: isbetter timestring, timestring
# returns 1 if the first timestring is a better time

sub is_better {
	
	my $first_secs=0; 
	my $second_secs=0; 
	my $next=0;

	@split_first_arg = split /:/,shift(@_) ;
	while($next = shift(@split_first_arg)){
		$first_secs = 60*$first_secs + $next;
	}
	@split_second_arg = split /:/,shift(@_) ;
	while($next = shift(@split_second_arg)){
		$second_secs = 60*$second_secs + $next;
	}

	# If the second arg is non-existant, we take it to mean no time
	if(!$second_secs){return 0;}
	return ( $first_secs < $second_secs) ? 1 : 0;
}

# Given a date string in the format m/d/y return an array 
# (m d yyyy)  Note that years may be passed in two digit format
# but are always returned in 4 digit format, with a 15 year Y2K window.
# Usage: parse_date datestring

sub parse_date {


	my ($mo, $day, $year) = split /\//, shift(@_);

	# Windowize to handle 2 digit dates in Y2K compliant way
	if( $year <= 99) {
		if($year < 70) {
			$year += 2000;
		}
		else {
			$year += 1900;
		}
	}
	return ($mo, $day, $year);
}


# Look up a person in the runners database by name, return their age group
# string ( e.g, m49 )
# usage: lookup_age name, date

sub lookup_age {

	my $name = shift(@_);
	my $age = 0;
	my $ags = "44"; # age group string
	my $mo_dob = $runners{$name}[0]; 
	my $day_dob= $runners{$name}[1];
	my $year_dob = $runners{$name}[2];
        my $sex =  $runners{$name}[3]; 
	my ($mo_now, $day_now, $year_now) = parse_date shift(@_);

	if($open_ag){ # generating open records, so no agegroups needed.
		return $sex."29";
	}
	
	# Convert dob year to 4 digits
	# This will break for very young runners after 2000 unless we start
        # recording DOBs to 4 digits
	if($year_dob <= 99){
		$year_dob += 1900;
	}
	# if now is on or after their birthday, then their age
        # is year_now - year_dob; otherwise, it is one fewer

	if ($mo_now > $mo_dob || ($mo_now == $mo_dob && day_now >= day_dob)){
		 $age = $year_now - $year_dob;
	}
	else {
		 $age =  $year_now - $year_dob - 1;
	}
	while(1){  # switch kludge
		if($age < 30) {$ags = 29 ; last;}
		if($age < 35) {$ags = 34 ; last;}
		if($age < 40) {$ags = 39 ; last;}
		if($age < 45) {$ags = 44 ; last;}
		if($age < 50) {$ags = 49 ; last;}
		if($age < 55) {$ags = 54 ; last;}
		if($age < 60) {$ags = 59 ; last;}
		if($age < 65) {$ags = 64 ; last;}
		if($age < 70) {$ags = 69 ; last;}
		if($age < 75) {$ags = 74 ; last;}
		if($age < 80) {$ags = 79 ; last;}
		if($age < 85) {$ags = 84 ; last;}
		if($age < 90) {$ags = 89 ; last;}
		$ags = "94";
		last;
	}
	return $sex.$ags;
}
