#!/usr/bin/env perl
use 5.014;
use strict;
use warnings;
use Math::Complex;

my $iterations = 256;
my $width  = int `tput cols`;
my $height = int `tput lines`;

my @view_x = (-2, 1);
my @view_y = (-1, 1);

sub is_in_cardioid {
    my $z = shift;
    my $q = (Re($z) - 0.25)**2 + Im($z)**2;
    return $q*($q + (Re($z) - 0.25)) < 0.25 * Im($z)**2;
}

sub is_in_2bulb {
    my $z = shift;
    return (Re($z) + 1)**2 + Im($z)**2 < 1/16;
}

sub mandelbrot {
    my ($x, $y) = @_;
    my $c = $x + i*$y;
    return 0 if is_in_cardioid($c) or is_in_2bulb($c);

    my $z = 0;
    for(my $i=0; $i<$iterations; $i++){
        return $i if abs $z > 2;
        $z = $z**2 + $c;
    }
    return 0;
}

#Initialise terminal. This says "save state, clear the screen and make the
#cursor invisible."
system 'tput smcup';
system 'tput reset';
system 'tput civis';

my $colour_256 = 1;
$| = 1;
for(my $k=0; $k < 10; $k++){
    system 'tput cup 0 0';
    for(my $j=0; $j<$height; $j++){
        for(my $i=0; $i<$width; $i++){
            my $dx = ($view_x[1] - $view_x[0]) / $width;
            my $dy = ($view_y[1] - $view_y[0]) / $height;
            my $x = $view_x[0] + $dx * $i;
            my $y = $view_y[0] + $dy * $j;
          
            if((my $its = mandelbrot($x, $y)) == 0){
                print "\033[1;0m*";
            }else{
                if($colour_256){
                    my $colcode = ($its > 255) ? 255 : $its;
                    print "\033[38;5;${its}m.";
                }else{
                    my $colcode = ($its > 7) ? 7 : $its;
                    print "\033[1;3${its}m.";
                }
            }
        }
    }
    my $delta_x = ($view_x[1] - $view_x[0]) * 0.33;
    for(@view_x){ $_ *= 0.75; $_ -= $delta_x }
    for(@view_y){ $_ *= 0.75 }
    sleep 2;
}

#system 'tput rmcup';
system 'tput cnorm';
