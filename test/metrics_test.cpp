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
	Mnemonic:	metric_test.cpp
	Abstract:	This is the unit test driver for the metrics class.

	Date:		20 July 2020
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

#include "../src/messaging/callback.hpp"
#include "../src/messaging/default_cb.hpp"
#include "../src/messaging/message.hpp"
#include "../src/messaging/messenger.hpp"
#include "../src/messaging/msg_component.hpp"
#include "../src/alarm/alarm.hpp"
#include "../src/xapp/xapp.hpp"

#include "../src/metrics/metrics.hpp"		// overtly pull the code under test to get coverage opts
#include "../src/messaging/messenger.cpp"
#include "../src/metrics/metrics.cpp"

#include "ut_support.cpp"

int main( int argc, char** argv ) {
	int		errors = 0;
	std::shared_ptr<Xapp> x;
	std::shared_ptr<xapp::Metrics> m;

	set_test_name( "metrics_test" );

	x = std::shared_ptr<Xapp>( new Xapp( "4560", true ) );
	if( fail_if( x == NULL, "could not allocate new xapp"  ) ) {
		announce_results( 1 );
		return 1;
	}

	m = x->Alloc_metrics( );
	if( fail_if( m == NULL, "could not allocate a metric object" ) ) {
		announce_results( errors );
		return errors;
	}

	m->Push_data( "barney_balance", 216.49 );
	m->Push_data( "fred_balance", 760.88 );
	m->Send( );

	// ensure data is cleared after first send
	m->Push_data( "barney_balance", 216.49 );
	m->Push_data( "fred_balance", 760.88 );
	m->Push_data( "wilma_balance", 1986.0430 );
	m->Send();

	m->Send();				// shouldn't really send


	// drive alternate builders
	m = x->Alloc_metrics( "different-source" );
	if( fail_if( m == NULL, "could not allocate a metric object" ) ) {
		announce_results( errors );
		return errors;
	}

	m->Push_data( "wilma_balance", 1986.0430 );
	m->Send();

	m = x->Alloc_metrics( "different-app", "different-source" );
	if( fail_if( m == NULL, "could not allocate a metric object" ) ) {
		announce_results( errors );
		return errors;
	}

	m->Push_data( "wilma_balance", 1986.0430 );
	m->Push_data( "pebbles_balance", 1982.0614 );
	m->Send();


	m->Set_reporter( "set-reporter" );
	m->Set_source( "set-source" );


	// drive move/copy adjunct functions

	xapp::Metrics b = *m.get();				// force the move/copy operator functions to trigger
	xapp::Metrics c( NULL );				// a useless metric without a message
	xapp::Metrics f( NULL, "source" );		// a useless metric to drive direct construction
	c = *m.get();							// drive copy = operator

	b = std::move( c );						// move = operator
	xapp::Metrics d = std::move( b );			// move constructor


	// ---------------------------- end housekeeping ---------------------------
	announce_results( errors );
	return !!errors;
}
