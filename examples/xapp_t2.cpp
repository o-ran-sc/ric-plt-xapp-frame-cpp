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
		
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <memory>

#include "ricxfcpp/xapp.hpp"

// ----------------------------------------------------------

extern int main( int argc, char** argv ) {
	std::unique_ptr<Xapp> xfw;
	std::unique_ptr<Message> msg;
	Msg_component payload;				// special type of unique pointer to the payload 

	int mtype;
	int	sz;
	int i;
	int ai;
	int response_to = 0;				// max timeout wating for a response
	char*	port = (char *) "4555";
	

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

			case 't':					// timeout in seconds; we need to convert to ms for rmr calls
				response_to = atoi( argv[ai+1] ) * 1000;
				ai++;
				break;
		}
		ai++;
	}

	fprintf( stderr, "<XAPP> response timeout set to: %d\n", response_to );
	fprintf( stderr, "<XAPP> listening on port: %s\n", port );

	xfw = std::unique_ptr<Xapp>( new Xapp( port, true ) );		// new xAPP thing; wait for a route table
	msg = xfw->Alloc_msg( 2048 );

	for( i = 0; i < 100; i++ ) {
		sz = msg->Get_available_size();			// we'll reuse a message if we received one back; ensure it's big enough
		if( sz < 2048 ) {
			fprintf( stderr, "<SNDR> fail: message returned did not have enough size: %d [%d]\n", sz, i );
			exit( 1 );
		}

		payload = msg->Get_payload();											// direct access to payload
		snprintf( (char *) payload.get(), 2048, "This is message %d\n", i );	// something silly to send

		// payload updated in place, nothing to copy from, so payload parm is nil
		if ( ! msg->Send_msg( 1, Message::NO_SUBID, strlen( (char *) payload.get() )+1, NULL )) {
			fprintf( stderr, "<SNDR> send failed: %d\n", i );
		}

		msg = xfw->Receive( response_to );
		if( msg != NULL ) {
			mtype = msg->Get_mtype();
			payload = msg->Get_payload();
			fprintf( stderr, "got: mtype=%d payload=(%s)\n", mtype, (char *) payload.get() );
		} else {
			msg = xfw->Alloc_msg( 2048 );				// nothing back, need a new message to send
		}

		sleep( 1 );
	}
}
