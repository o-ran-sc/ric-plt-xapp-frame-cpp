#!/usr/bin/env bash
# vim: ts=4 sw=4 noet:

#
#	Mnemonic:	scrub_gcov.sh
#	Abstract:	Gcov (sadly) outputs for any header file that we pull.
#				this scrubs any gcov that doesnt look like it belongs
#				to our code.
#
#	Date:		24 March 2020
#	Author:		E. Scott Daniels
# -----------------------------------------------------------------------------


# Make a list of our modules under test so that we don't look at gcov
# files that are generated for system lib headers in /usr/*
# (bash makes the process of building a list of names  harder than it 
# needs to be, so use caution with the printf() call.)
#
function mk_list {
	for f in *.gcov
	do
		if ! grep -q "Source:\.\./src"  $f
		then
			printf "$f "		# do NOT use echo or add \n!
		fi
	done 
}


list=$( mk_list )
if [[ -n $list ]]
then
	rm $list
fi
