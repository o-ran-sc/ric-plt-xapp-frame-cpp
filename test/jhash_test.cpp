// vim: ts=4 sw=4 noet :
/*
==================================================================================
       Copyright (c) 2020 Nokia
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
	Mnemonic:	json_test.cpp
	Abstract:	Unit test for the json module. This expects that a static json
				file exist in the current directory with a known set of fields,
				arrays and objects that can be sussed out after parsing. The
				expected file is test.json.

	Date:		26 June 2020
	Author:		E. Scott Daniels
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <string>
#include <memory>


/*
	Very simple file reader. Reads up to 8k into a single buffer and
	returns the buffer as char*.  Easier to put json test things in
	a file than strings.
*/
static char* read_jstring( char* fname ) {
	char*	rbuf;
	int	fd;
	int len;

	rbuf = (char *) malloc( sizeof( char ) * 8192 );
	fd = open( fname, O_RDONLY, 0 );
	if( fd < 0  ) {
		fprintf( stderr, "<ABORT> can't open test file: %s: %s\n", fname, strerror( errno ) );
		exit( 1 );
	}

	len = read( fd, rbuf, 8190 );
	if( len < 0  ) {
		close( fd );
		fprintf( stderr, "<ABORT> read from file failed: %s: %s\n", fname, strerror( errno ) );
		exit( 1 );
	}

	rbuf[len] = 0;
	close( fd );

	return rbuf;
}

// this also tests jwrapper.c but that is built as a special object to link in
// rather than including here.
//
#include "../src/json/jhash.hpp"
#include "../src/json/jhash.cpp"

#include "ut_support.cpp"

int main( int argc, char** argv ) {
	int		errors = 0;
	xapp::Jhash*	jh;
	char*	jstr;
	std::string	sval;
	double	val;
	bool	state;
	int		i;
	int		len;
	int		true_count = 0;

	set_test_name( "jhash_test" );
	jstr = read_jstring( (char *) "test.json" );			// read and parse the json

	fprintf( stderr, "read: (%s)\n", jstr );

	jh = new xapp::Jhash( jstr );
	free( jstr );

	if( jh == NULL ) {
		fprintf( stderr, "<FAIL> could not parse json string from: test.json\n" );
		exit( 1 );
	}


	sval = jh->String( (char *) "meeting_day" );
	fprintf( stderr, "<INFO> sval=(%s)\n", sval.c_str() );
	errors += fail_if( sval.compare( "" ) == 0, "did not get meeting day string" );
	errors += fail_if( sval.compare( "Tuesday" ) != 0, "meeting day was not expected string" );

	sval = jh->String( (char *) "meeting_place" );
	fprintf( stderr, "<INFO> sval=(%s)\n", sval.c_str() );
	errors += fail_if( sval.compare( "" ) == 0, "did not get meeting place" );
	errors += fail_if( sval.compare( "16801 East Green Drive" ) != 0, "meeting place stirng was not correct" );

	state = jh->Exists( (char *) "meeting_place" );
	errors += fail_if( !state, "test for meeting place exists did not return true" );

	state = jh->Exists( (char *) "no-name" );
	errors += fail_if( state, "test for non-existant thing returned true" );

	state = jh->Is_missing( (char *) "no-name" );
	errors += fail_if( !state, "missing test for non-existant thing returned false" );

	state = jh->Is_missing( (char *) "meeting_place" );
	errors += fail_if( state, "missing test for existing thing returned true" );

	val = jh->Value( (char *) "lodge_number" );
	errors += fail_if( val != 41.0, "lodge number value was not correct" );

	val = jh->Value( (char *) "monthly_dues" );
	fprintf( stderr, "<INFO> got dues: %.2f\n", val );
	errors += fail_if( val != (double) 43.5, "lodge dues value was not correct" );

	len = jh->Array_len( (char *) "members" );
	fprintf( stderr, "<INFO> got %d members\n", len );
	errors += fail_if( len != 4, "array length was not correct" );
	if( len > 0 ) {
		for( i = 0; i < len; i++ ) {
			if( ! jh->Set_blob_ele( (char *) "members", i ) ) {
				errors++;
				fprintf( stderr, (char *) "couldn't set blob for element %d\n", i );
			} else {
				fprintf( stderr, (char *) "<INFO> testing element %d of %d\n", i, len );

				state = jh->Is_value( (char *) "age" );
				errors += fail_if( !state, "is value test for age returned false" );
				state = jh->Is_value( (char *) "married" );
				errors += fail_if( state, "is value test for married returned true" );

				state = jh->Is_string( (char *) "occupation" );
				errors += fail_if( !state, "is string test for spouse returned false" );
				state = jh->Is_string( (char *) "married" );
				errors += fail_if( state, "is string test for married returned true" );

				state = jh->Is_bool( (char *) "married" );
				errors += fail_if( !state, "is bool test for married returned false" );
				state = jh->Is_bool( (char *) "occupation" );
				errors += fail_if( state, "is bool test for spouse returned true" );

				val = jh->Value( (char *) "age" );
				fprintf( stderr, "<INFO> got age: %.2f\n", (double) val );
				errors += fail_if( val < 0,  "age value wasn't positive" );

				sval = jh->String( (char *) "name" );
				fprintf( stderr, "<INFO> sval=(%s)\n", sval.c_str() );
				errors += fail_if( sval.compare( "" ) == 0, "no name found in element" );

				if( jh->Bool( (char *) "married" ) ) {
					true_count++;
				}
			}

			jh->Unset_blob();		// must return to root
		}

		fprintf( stderr, "<INFO> true count = %d\n", true_count );
		errors += fail_if( true_count != 3, "married == true count was not right" );
	}

	state = jh->Set_blob( (char *) "no-such-thing" );
	errors += fail_if( state, "setting blob to non-existant blob returned true" );

	state = jh->Set_blob( (char *) "grand_poobah" );
	errors += fail_if( !state, "setting blob to existing blob failed" );
	if( state ) {
		sval  = jh->String( (char *) "elected" );
		fprintf( stderr, "<INFO> sval=(%s)\n", sval.c_str() );
		errors += fail_if( sval != "February 2019", "blob 'elected' didn't return the expected string" );

		state = jh->Exists( (char *) "monthly_dues" );
		errors += fail_if( state, "blob that shouldn't have a field reports it does" );

		jh->Unset_blob( );							// ensure that this is found once we unset to root
		state = jh->Exists( (char *) "monthly_dues" );
		errors += fail_if( !state, "after rest, root blob, that should have a field, reports it does not" );
	}


	// ---- test array element value type checks -------------------------------------------------
	state = jh->Is_string_ele( (char *) "sponser", 1 );
	errors += fail_if( !state, "string element check on sponser failed" );
	state = jh->Is_string_ele( (char *) "current_on_dues", 1 );
	errors += fail_if( state, "string element check on non-stirng element returned true" );

	state = jh->Is_value_ele( (char *) "dues_assistance", 1 );
	errors += fail_if( !state, "value element type check on value element reported false" );
	state = jh->Is_value_ele( (char *) "current_on_dues", 1 );
	errors += fail_if( state, "value element type check on non-value element returned true" );

	state = jh->Is_bool_ele( (char *) "current_on_dues", 1 );
	errors += fail_if( !state, "string element check on sponser failed" );
	state = jh->Is_bool_ele( (char *) "sponser", 1 );
	errors += fail_if( state, "string element check on non-stirng element returned true" );

	state = jh->Is_null( (char *) "nvt" );
	errors += fail_if( !state, "test for nil value returned false" );
	state = jh->Is_null( (char *) "lodge_number" );
	errors += fail_if( state, "nil test for non-nil value returned true" );

	state = jh->Is_null_ele( (char *) "nvat", 0 );
	errors += fail_if( !state, "test for nil array element value returned false" );


	// ---- test sussing of elements from arrays -------------------------------------------------
	sval = jh->String_ele( (char *) "sponser", 1 );
	errors += fail_if( sval.compare( "" ) == 0, "get string element failed for sponser (empty string)" );
	errors += fail_if( sval.compare( "slate" ) != 0, "get string element failed for sponser (wrong value for[1])" );

	sval = jh->String_ele( (char *) "sponser", 0 );
	errors += fail_if( sval.compare( "slate" ) != 0, "get string element failed for sponser (wrong value for [0])" );

	sval = jh->String_ele( (char *) "sponser", 3 );
	errors += fail_if( sval.compare( "brick" ) != 0, "get string element failed for sponser (wrong value for [3])" );

	val = jh->Value_ele( (char *) "dues_assistance", 1 );
	errors += fail_if( val == 0.0, "get value element for dues_assistance was zero" );

	state = jh->Bool_ele( (char *) "current_on_dues", 1 );
	errors += fail_if( state, "bool ele test returned true for a false value" );
	state = jh->Bool_ele( (char *) "current_on_dues", 0 );
	errors += fail_if( !state, "bool ele test returned false for a true value" );


	val = jh->Value( (char *) "timestamp" );
	fprintf( stderr, "<INFO> timestamp: %.10f\n", val );

	jh->Dump();			// for coverage of debug things


	//  ----- jhashes can be moved, drive that logic for coverage
	xapp::Jhash  j2( "{}" );

	xapp::Jhash j1 = std::move( *jh );				// drives move constructor function
	j2 = std::move( j1 );						// drives move operator function



	delete jh;

	fprintf( stderr, "<INFO> testing for failures; jwrapper error and warning messages expected\n" );
	// ---- these shouild all fail to parse, generate warnings to stderr, and drive error handling coverage ----
    jh = new xapp::Jhash( (char *) "{ \"bad\": [ [ 1, 2, 3 ], [ 3, 4, 5]] }" );		// drive the exception process for bad json
	delete jh;

    jh = new xapp::Jhash( (char *) " \"bad\":  5 }" );			// no opening brace
	state = jh->Parse_errors();
	errors += fail_if( !state, "parse errors check returned false when known errors exist" );
	delete jh;

    jh = new xapp::Jhash( (char *) "{ \"bad\":  fred }" );		// no quotes
	delete jh;

    jh = new xapp::Jhash( (char *) "{ \"bad:  456, \"good\": 100 }" );			// missing quote; impossible to detect error
	jh->Dump();																// but dump should provide details
	fprintf( stderr, "<INFO> good value=%d\n", (int) val );
	delete jh;


	// ---------------------------- end housekeeping ---------------------------
	announce_results( errors );
	return !!errors;
}
