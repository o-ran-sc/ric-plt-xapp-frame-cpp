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

	Caution:	This example code is pulled directly into one or more of the documents
				(starting from the "start-example" tag below.  Use caution with
				line lengths and contents because of the requirement that this
				be documentation as well as working code.
*/
// start-example
#include <stdio.h>

#include "ricxfcpp/message.hpp"
#include "ricxfcpp/msg_component.hpp"
#include <ricxfcpp/metrics.hpp>
#include "ricxfcpp/xapp.hpp"

// counts; not thread safe
long cb1_count = 0;
long cb2_count = 0;
long cbd_count = 0;

long cb1_lastts = 0;
long cb1_lastc = 0;

/*
	Respond with 2 messages for each type 1 received
	Send metrics every 1000 messages.
*/
void cb1( xapp::Message& mbuf, int mtype, int subid, int len,
			xapp::Msg_component payload,  void* data ) {
	long now;
	long total_count;

	// illustrate that we can use the same buffer for 2 rts calls
	mbuf.Send_response( 101, -1, 5, (unsigned char *) "OK1\n" );
	mbuf.Send_response( 101, -1, 5, (unsigned char *) "OK2\n" );

	cb1_count++;

    if( cb1_count % 1000 == 0 && data != NULL ) {   // send metrics every 1000 messages
        auto x = (Xapp *) data;
        auto msgm = std::shared_ptr<xapp::Message>( x->Alloc_msg( 4096 ) );

        auto m = std::unique_ptr<xapp::Metrics>( new xapp::Metrics( msgm ) );
        m->Push_data( "tst_cb1", (double) cb1_count );
        m->Push_data( "tst_cb2", (double) cb2_count );
        m->Send();
    }
}

// just count messages
void cb2( xapp::Message& mbuf, int mtype, int subid, int len,
			xapp::Msg_component payload,  void* data ) {
	cb2_count++;
}

// default to count all unrecognised messages
void cbd( xapp::Message& mbuf, int mtype, int subid, int len,
			xapp::Msg_component payload,  void* data ) {
	cbd_count++;
}

int main( int argc, char** argv ) {
	Xapp* x;
	char*	port = (char *) "4560";
	int ai = 1;							// arg processing index
	int nthreads = 1;

	// very simple flag processing (no bounds/error checking)
	while( ai < argc ) {
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
	x->Add_msg_cb( 1, cb1, x );		// register callbacks
	x->Add_msg_cb( 2, cb2, NULL );
	x->Add_msg_cb( x->DEFAULT_CALLBACK, cbd, NULL );

	x->Run( nthreads );				// let framework drive
	// control should not return
}
