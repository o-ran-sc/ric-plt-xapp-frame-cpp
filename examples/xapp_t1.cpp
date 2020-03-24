// vi: ts=4 sw=4 noet:
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
	Mnemonic:	xapp_t1.cpp
	Abstract:	This is a simple demo xapp which registers 3 callback functions
				(2 for specific message types, and one default callback). It
				defaults to starting 1 thread, but can cause the xapp environment
				to start n listeners.  When running 1 thread it should emit 
				message countes ever few seconds with a crudly computed msg/sec 
				receive rate value.

				In addition, the callback for message type 1 will send two response
				messages to verify/illustrate that the same received buffer can
				be used to construct multiple messages and given to the return to
				sender function in RMR for multiple message reponse generation.

	Date:		18 March 2020
	Author:		E. Scott Daniels
		
*/
#include <stdio.h>

#include "ricxfcpp/message.hpp"
#include "ricxfcpp/msg_component.hpp"
#include "ricxfcpp/xapp.hpp"

// ----------------------------------------------------------

// counts/time values for crude rate estimation; only valid when threads == 1
long cb1_count = 0;
long cb2_count = 0;
long cbd_count = 0;

long cb1_lastts = 0;
long cb1_lastc = 0;

void cb1( Message& mbuf, int mtype, int subid, int len, Msg_component payload,  void* data ) {
	long now;
	long total_count;

	//fprintf( stderr, "callback 1 got a message type = %d len = %d\n", mtype, len );
	mbuf.Send_response( 101, -1, 5, (unsigned char *) "OK1\n" );	// validate that we can use the same buffer for 2 rts calls
	mbuf.Send_response( 101, -1, 5, (unsigned char *) "OK2\n" );
	cb1_count++;
	
	now = time( NULL );
	if( now - cb1_lastts > 5 ) {				// crude rate estimation starting with second timer pop
		if( cb1_lastts ) {
			total_count = cb1_count + cb2_count;
			fprintf( stderr, "cb1: %ld  diff=%ld ~rate=%ld\n", total_count, now - cb1_lastts, (total_count-cb1_lastc)/(now - cb1_lastts) );
			cb1_lastc = total_count;
		}
		cb1_lastts = now;
	}
}

void cb2( Message& mbuf, int mtype, int subid, int len, Msg_component payload,  void* data ) {
	//fprintf( stderr, "callback 2 got a message type = %d len = %d\n", mtype, len );
	//mbuf.Send_msg( 101, -1, 4, (unsigned char *) "HI\n" );		// send, including the trailing 0
	cb2_count++;
}

void cbd( Message& mbuf, int mtype, int subid, int len, Msg_component payload,  void* data ) {
	//fprintf( stderr, "default callback  got a message type = %d len = %d\n", mtype, len );
	cbd_count++;
}

int main( int argc, char** argv ) {
	Xapp* x;
	char*	port = (char *) "4560";
	int ai = 1;							// arg processing index
	int nthreads = 1;

	ai = 1;
	while( ai < argc ) {				// very simple flag processing (no bounds/error checking)
		if( argv[ai][0] != '-' )  {
			break;
		}

		switch( argv[ai][1] ) {			// we only support -x so -xy must be -x -y
			case 'p': 
				port = argv[ai+1];	
				ai++;
				break;

			case 't':
				nthreads = atoi( argv[ai+1] );
				ai++;
				break;
		}

		ai++;
	}
	
	fprintf( stderr, "<XAPP> listening on port: %s\n", port );
	fprintf( stderr, "<XAPP> starting %d threads\n", nthreads );

	x = new Xapp( port, true );
	x->Add_msg_cb( 1, cb1, NULL );
	x->Add_msg_cb( 2, cb2, NULL );
	x->Add_msg_cb( x->DEFAULT_CALLBACK, cbd, NULL );

	x->Run( nthreads );
}
