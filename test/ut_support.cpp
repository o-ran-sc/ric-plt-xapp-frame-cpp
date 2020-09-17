// vi: ts=4 sw=4 noet:
/*
==================================================================================
    Copyright (c) 2020 AT&T Intellectual Property.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
==================================================================================
*/

/*
    Mnemonic:	ut_support.c
    Abstract:	Tools to make unit testing easier.

    Date:       16 June 2020
    Author:     E. Scott Daniels
*/

#include <string>

static std::string test_name = "unknown";
static int ut_tests_driven = 0;

// -------------------------------------------------------------------------------

/*
	Set the name of the current tester
*/
extern void set_test_name( std::string name ) {
	test_name = name;
}

/*
	Returns 1 if the condition is true (not zero)
*/
extern int fail_if( int cond, std::string reason ) {
	ut_tests_driven++;

	if( cond ) {
		fprintf( stderr, "<FAIL> %s: %s\n", test_name.c_str(), reason.c_str() );
		return 1;
	}

	return 0;
}

/*
	Returns 1 if the condition is false.
*/
extern int fail_if_false( int cond, std::string reason ) {
	ut_tests_driven++;

	if( !cond ) {
		fprintf( stderr, "<FAIL> %s: %s\n", test_name.c_str(), reason.c_str() );
		return 1;
	}

	return 0;
}

extern void announce_results( int errors ) {
	fprintf( stderr, "<SUMMARY> %s %d tests drivn, %d passed, %d failed\n",
		test_name.c_str(), ut_tests_driven, ut_tests_driven - errors, errors );

	if( errors > 0 ) {
		fprintf( stderr, "<FAIL> %s: failed with %d errors\n", test_name.c_str(), errors );
	} else {
		fprintf( stderr, "<PASS> %s: passed with\n", test_name.c_str() );
	}
}
