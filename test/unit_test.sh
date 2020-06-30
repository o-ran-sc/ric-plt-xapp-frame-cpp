#!/usr/bin/env bash
# vim: ts=4 sw=4 noet:

#==================================================================================
#       Copyright (c) 2020 Nokia
#       Copyright (c) 2020 AT&T Intellectual Property.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#==================================================================================

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
make tests >/tmp/PID$$.log 2>&1
abort_if_error $? "unable to make"
#make unit_test >/tmp/PID$$.log 2>&1

spew="cat"

for x in unit_test jhash_test
do
	./$x >/tmp/PID$$.log 2>&1
	abort_if_error $? "test failed: $x"
	gcov $x.c >/dev/null 2>&1
done

# wrapper_test is driven by jhash_test, but must be covered explicitly
gcov jwrapper_test.c >/dev/null 2>&1

./scrub_gcov.sh								# remove cruft

list=$( mk_list )
echo ""
echo "[INFO] coverage stats, discounted (raw), for the various modules:"
./parse_gcov.sh $list						# generate simple, short, coverage stats

rm -f /tmp/PID$$.*

