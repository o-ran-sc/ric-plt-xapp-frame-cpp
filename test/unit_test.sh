#!/usr/bin/env bash
# vim: ts=4 sw=4 noet:

#
#	Mnemonic:	unit_test.sh
#	Abstract:	This drives the unit tests and combs out the needed .gcov
#				files which are by some magic collected for Sonar.
#
#	Date:		23 March 2020
#	Author:		E. Scott Daniels
# -----------------------------------------------------------------------------


# Make a list of our modules under test so that we don't look at gcov
# files that are generated for system lib headers in /usr/*
# (bash makes the process of building a list of names  harder than it 
# needs to be, so use caution with the printf() call.)
#
function mk_list {
	grep -l "Source:\.\./src"  *.gcov | while read f
	do
		printf "$f "		# do NOT use echo or add \n!
	done 
}

function abort_if_error {
	if (( $1 == 0 ))
	then
		return
	fi

	if [[ -n /tmp/PID$$.log ]]
	then
		$spew /tmp/PID$$.log
	fi
	echo "abort: $2"

	rm -f /tmp/PID$$.*
	exit 1
}

# -------------------------------------------------------------------------

spew="cat"					# default to dumping all make output on failure (-q turns it to ~40 lines)

while [[ $1 == "-"* ]]
do
	case $1 in
		-q) spew="head -40";;
		-v)	spew="cat";;
	esac

	shift
done

make nuke >/dev/null
make unit_test >/tmp/PID$$.log 2>&1
abort_if_error $? "unable to make"

spew="cat"
./unit_test >/tmp/PID$$.log 2>&1
abort_if_error $? "unit test failed"

gcov unit_test >/tmp/PID$$.gcov_log 2>&1	# suss out our gcov files
./scrub_gcov.sh								# remove cruft

list=$( mk_list )
./parse_gcov.sh $list						# generate simple, short, coverage stats

rm -f /tmp/PID$$.*

