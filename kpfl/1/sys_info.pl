#!/usr/bin/perl -w
use strict;
use warnings;

print "Welcome to use the system info utility\n\n";

print "## CPU INFO ##\n";
open CPU, "</proc/cpuinfo";
my ($cpu_vendor, $cpu_name, $cores);

$cores = 0;
while(<CPU>){
    if(/processor/) {
        $cores++;
    } elsif (/vendor_id\s+:\s+(\w+)/) {
        $cpu_vendor = $1;
    } elsif (/model name\s+:\s+(.+)$/) {
        $cpu_name = $1;
    }
}

print "CPU Vendor: $cpu_vendor\nCPU name: $cpu_name\nCPU cores: $cores\n\n";

close CPU;

print "## KERN VERSION ##\n";
system("cat /proc/version");
print "\n";

open UPTIME, "</proc/uptime";

my ($total, $days, $hours, $mins, $secs);
my @input; 
$total = <UPTIME>;
@input = split ' ', $total;
$total = $input[0];

$days = $total / (3600 * 24);
$total = $total % (3600 * 24);

$hours = $total / 3600;
$total = $total % 3600;

$mins = $total / 60;
$secs = $total % 60;

print "## SYSTEM UPTIME ##\n";
printf "%02d:%02d:%02d:%02d\n\n", $days, $hours, $mins, $secs;

close UPTIME;
