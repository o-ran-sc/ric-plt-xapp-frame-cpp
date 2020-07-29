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
	Mnemonic:	config_test.cpp
	Abstract:	Unit test to drive the config functions.

	Date:		28 July 2020
	Author:		E. Scott Daniels
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <string>
#include <memory>

#include "../src/xapp/xapp.hpp"

//#include "../src/json/jhash.hpp"
//#include "../src/json/jhash.cpp"

#include "../src/config/config.hpp"		// include things directly under test
#include "../src/config/config_cb.hpp"
#include "../src/config/config.cpp"
#include "../src/config/config_cb.cpp"

#include "ut_support.cpp"

char* data = (char *) "some data to validate in callback";
bool	callback_driven = false;
int		cb_errors = 0;


/*
	Notification callback to see that it is driven on change.
*/
void ncb( xapp::Config& c, void* data ) {
	cb_errors += fail_if( data == NULL, "callback function did not get a good pointer" );

	auto v = c.Get_control_value( "measurement_interval" );
	cb_errors += fail_if( v != 1000, "measurement value in new file wasn't different" );

	callback_driven = true;
}

int main( int argc, char** argv ) {
	int		errors = 0;

	set_test_name( "config_test" );


	auto x = new Xapp( (char*) "43086", false );
	if(	fail_if( x == NULL, "could not allocate xapp" )  > 0 ) {
		announce_results( 1 );
		exit( 1 );
	}

	auto c = new xapp::Config( "config1.json" );
	errors += fail_if( c == NULL, "unable to allocate a config with alternate name" );

	auto s = c->Get_control_str( "ves_collector_address" );
	errors += fail_if( s.empty(), "epxected control string not found" );
	fprintf( stderr, "<INFO> collector address string var: %s\n", s.c_str() );

	s = c->Get_port( "rmr-data-out" );
	errors += fail_if( s.empty(), "epxected port string not found" );
	fprintf( stderr, "<INFO> port string var: %s\n", s.c_str() );

	s = c->Get_port( "no-interface" );
	errors += fail_if( ! s.empty(), "did not return empty when get port given an invalid name" );

	s = c->Get_control_str( "no-such-control" );
	errors += fail_if( ! s.empty(), "epxected empty string for missing control got a string" );
	if( ! s.empty() ) {
		fprintf( stderr, "<INFO> unexpected string for no such control name:  %s\n", s.c_str() );
	}

	auto v = c->Get_control_value( "measurement_interval" );
	errors += fail_if( v == 0.0, "epxected measurement interval control value not found" );

	auto b = c->Get_control_bool( "debug_mode" );
	errors += fail_if( b == false, "epxected debug mode control boolean not found or had wrong value" );


	// ----- test sussing path and using default name ----------------------------------
	fprintf( stderr, "<INFO> starting test on second config load\n" );

	c = new xapp::Config(  );							// drive for coverage

	setenv( (char *) "XAPP_DESCRIPTOR_PATH", ".", 1 );
	c = new xapp::Config(  );

	s = c->Get_control_str( "ves_collector_address" );
	errors += fail_if( s.empty(), "epxected collector address control string not found" );
	fprintf( stderr, "<INFO> string var: %s\n", s.c_str() );

	v = c->Get_control_value( "measurement_interval" );
	errors += fail_if( v == 0.0, "epxected measurement interval control value not found" );

	b = c->Get_control_bool( "debug_mode" );
	errors += fail_if( b == false, "epxected debug mode control boolean not found" );


	auto cs = c->Get_contents();
	if( fail_if( cs.empty(), "get contents returned an empty string" ) == 0 ) {
		fprintf( stderr, "<INFO> contents from file: %s\n", cs.c_str() );
		fprintf( stderr, "<INFO> ---------------------\n" );
	} else {
		errors++;
	}


	// -------------- force callback to drive and test ---------------------------------

	c->Set_callback( ncb, data );						// for coverage on ./name parsing

	c = new xapp::Config( "config1.json" );
	c->Set_callback( ncb, data );

	fprintf( stderr, "<INFO> sleeping to give callback time to be initialsed\n" );
	sleep( 4 );
	if( rename( (char *) "./config-file.json", (char *) "./tmp-config.json" ) == 0 ) {		// rename (should not cause callback)
		fprintf( stderr, "<INFO> file moved; sleeping a bit\n" );
		sleep( 3 );
		errors += fail_if( callback_driven, "callback was driven when file was deleted/moved away" );

		if( rename( (char *) "./tmp-config.json", (char *) "./config-file.json" ) == 0 ) {		// put it back to drive callback
			fprintf( stderr, "<INFO> sleeping to give callback time to be driven\n" );
			sleep( 3 );

			errors += fail_if( ! callback_driven, "callback was never executed" );
		} else {
			fprintf( stderr, "<WARN> attempt to move config file back failed: %s\n", strerror( errno ) );
		}
	} else {
		fprintf( stderr, "<WARN> attempt to move config file away failed: %s\n", strerror( errno ) );
	}


	// ----- force errors where we can -----------------------------------------
	fprintf( stderr, "<INFO> json parse errors expected to be reported now\n" );
	c = new xapp::Config( "not-there.json" );						// json parse will fail

	v = c->Get_control_value( "measurement_interval", 999 );
	errors += fail_if( v != 999.0, "value from non-existant file wasn't default" );

	s = c->Get_control_str( "ves_collector_address", "no-value" );
	errors += fail_if( s.compare( "no-value" ) != 0, "string from non-existant file wasn't default" );

	b = c->Get_control_bool( "debug_mode", false );
	errors += fail_if( b, "boolean from non-existant file wasn't default" );

	s = c->Get_port( "rmr-data-out" );
	errors += fail_if( !s.empty(), "get port from bad jsonfile returned value" );

	// ---------------------------- end housekeeping ---------------------------
	announce_results( cb_errors + errors );
	return !!errors;
}
