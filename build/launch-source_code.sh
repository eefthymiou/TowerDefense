#!/bin/sh
bindir=$(pwd)
cd /Users/johnefthymiou/Documents/university/7th senester/graphics/TowerDefense/source_code/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "xYES" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		/usr/local/bin/gdb -batch -command=$bindir/gdbscript --return-child-result /Users/johnefthymiou/Documents/university/7th\ senester/graphics/TowerDefense/build/source_code 
	else
		"/Users/johnefthymiou/Documents/university/7th\ senester/graphics/TowerDefense/build/source_code"  
	fi
else
	"/Users/johnefthymiou/Documents/university/7th\ senester/graphics/TowerDefense/build/source_code"  
fi
