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
	Mnemonic:	xapp_t2.cpp
	Abstract:	This is a simple demo xapp which controls it's own listen
				loop (does not register callbacks and does not invoke the
				run function in the xapp instance.

	Date:		18 March 2020
	Author:		E. Scott Daniels

	Caution:	This example code is pulled directly into one or more of the documents
				(starting from the "start-example" tag below.  Use caution with
				line lengths and contents because of the requirement that this
				be documentation as well as working code.
*/
// start-example

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <memory>

#include "ricxfcpp/xapp.hpp"

extern int main( int argc, char** argv ) {
	std::unique_ptr<Xapp> xfw;
	std::unique_ptr<xapp::Message> msg;
	xapp::Msg_component payload;				// special type of unique pointer to the payload

	int	sz;
	int len;
	int i;
	int ai;
	int response_to = 0;				// max timeout wating for a response
	char*	port = (char *) "4555";
	int	mtype = 0;
	int rmtype;							// received message type
	int delay = 1000000;				// mu-sec delay; default 1s


	// very simple flag processing (no bounds/error checking)
	while( ai < argc ) {
		if( argv[ai][0] != '-' )  {
			break;
		}

		// we only support -x so -xy must be -x -y
		switch( argv[ai][1] ) {
			// delay between messages (mu-sec)
			case 'd':
				delay = atoi( argv[ai+1] );
				ai++;
				break;

			case 'p':
				port = argv[ai+1];
				ai++;
				break;

			// timeout in seconds; we need to convert to ms for rmr calls
			case 't':
				response_to = atoi( argv[ai+1] ) * 1000;
				ai++;
				break;
		}
		ai++;
	}

	fprintf( stderr, "<XAPP> response timeout set to: %d\n", response_to );
	fprintf( stderr, "<XAPP> listening on port: %s\n", port );

	// get an instance and wait for a route table to be loaded
	xfw = std::unique_ptr<Xapp>( new Xapp( port, true ) );
	msg = xfw->Alloc_msg( 2048 );

	for( i = 0; i < 100; i++ ) {
		mtype++;
		if( mtype > 10 ) {
			mtype = 0;
		}

		// we'll reuse a received message; get max size
		sz = msg->Get_available_size();

		// direct access to payload; add something silly
		payload = msg->Get_payload();
		len = snprintf( (char *) payload.get(), sz, "This is message %d\n", i );

		// payload updated in place, prevent copy by passing nil
		if ( ! msg->Send_msg( mtype, xapp::Message::NO_SUBID,  len, NULL )) {
			fprintf( stderr, "<SNDR> send failed: %d\n", i );
		}

		// receive anything that might come back
		msg = xfw->Receive( response_to );
		if( msg != NULL ) {
			rmtype = msg->Get_mtype();
			payload = msg->Get_payload();
			fprintf( stderr, "got: mtype=%d payload=(%s)\n",
				rmtype, (char *) payload.get() );
		} else {
			msg = xfw->Alloc_msg( 2048 );
		}

		if( delay > 0 ) {
			usleep( delay );
		}
	}
}
