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
	Mnemonic:	Unit_test.cpp
	Abstract:	This is the unit test driver for the C++ xAPP framework

	Date:		20 March 2020
	Author: 	E. Scott Daniels
*/

#include <memory>


#include "../src/messaging/callback.hpp"
#include "../src/messaging/default_cb.hpp"
#include "../src/messaging/message.hpp"
#include "../src/messaging/messenger.hpp"
#include "../src/messaging/msg_component.hpp"
#include "../src/xapp/xapp.hpp"

#include "../src/messaging/callback.cpp"
#include "../src/messaging/default_cb.cpp"
#include "../src/messaging/message.cpp"
#include "../src/messaging/messenger.cpp"
#include "../src/xapp/xapp.cpp"

// -- callback functions to register
void cb1( Messenger& mr, Message& mbuf, int mtype, int subid, int len, Msg_component payload,  void* data ) {
}
void cb2( Messenger& mr, Message& mbuf, int mtype, int subid, int len, Msg_component payload,  void* data ) {
}
void cbd( Messenger& mr, Message& mbuf, int mtype, int subid, int len, Msg_component payload,  void* data ) {
}

void killer( std::shared_ptr<Xapp> x ) {
	fprintf( stderr, ">>>> killer is waiting in the shadows\n" );
	sleep( 5 );
	fprintf( stderr, ">>>> killer is on the loose\n" );
	x->Halt();
}

int main( int argc, char** argv ) {
	std::thread* tinfo;					// we'll start a thread that will shut things down after a few seconds
	std::unique_ptr<Message> msg;
	std::shared_ptr<Xapp> x;
	Msg_component payload;
	std::unique_ptr<unsigned char> ucs;
	unsigned char* new_payload;
	std::shared_ptr<unsigned char> new_p_ref;	// reference to payload to pass to send functions
	char*	port = (char *) "4560";
	int		ai = 1;							// arg processing index
	int		nthreads = 2;					// ensure the for loop is executed in setup
	int		i;
	int		errors = 0;

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
	
	x = std::shared_ptr<Xapp>( new Xapp( port, true ) );
	x->Add_msg_cb( 1, cb1, NULL );
	x->Add_msg_cb( 2, cb2, NULL );
	x->Add_msg_cb( -1, cbd, NULL );

	msg = x->Alloc_msg( 2048 );
	payload = msg->Get_payload();
	msg->Set_len( 128 );
	msg->Set_mtype( 100 );
	msg->Set_subid( -10 );

	ucs = msg->Copy_payload( );
	if( ucs == NULL ) {
		fprintf( stderr, "<FAIL> expected pointer to copy payload but got nil\n" );
		errors++;
	}

	ucs = msg->Get_meid();
	if( ucs == NULL ) {
		fprintf( stderr, "<FAIL> expected pointer to meid copy but got nil\n" );
		errors++;
	}

	ucs = msg->Get_src();
	if( ucs == NULL ) {
		fprintf( stderr, "<FAIL> expected pointer to src copy but got nil\n" );
		errors++;
	}

	i = msg->Get_available_size();
	if( i != 2048 ) {
		fprintf( stderr, "<FAIL> len expected payload avail size of 2048 but got %d\n", i );
		errors++;
	}

	i = msg->Get_mtype();
	if( i != 100 ) {
		fprintf( stderr, "<FAIL> expected mtype of 100 but got %d\n", i );
		errors++;
	}

	i = msg->Get_state( );
	if( i != 0 ) {
		fprintf( stderr, "<FAIL> expected state of 0 but got %d\n", i );
		errors++;
	}

	i = msg->Get_subid();
	if( i != -10 ) {
		fprintf( stderr, "<FAIL> expected subid of -10 but got %d\n", i );
		errors++;
	}

	i = msg->Get_len();
	if( i != 128 ) {
		fprintf( stderr, "<FAIL> len expected 128 but got %d\n", i );
		errors++;
	}

	msg->Send();				// generic send as is functions
	msg->Reply();

	new_payload = (unsigned char *) malloc( sizeof( unsigned char ) * 2048 );	// a large payload
	memset( new_payload, 0 , sizeof( unsigned char ) * 2048 );
	new_p_ref = std::shared_ptr<unsigned char>( new_payload );					// reference it for send calls

	msg->Set_meid( new_p_ref );

	msg->Send_msg( 255, new_p_ref );											// send without changing the message type/subid from above
	msg->Send_msg( 255, new_payload );											// drive the alternate prototype
	msg->Send_msg( 100, 1, 128, new_p_ref );									// send using just 128 bytes of payload
	msg->Send_msg( 100, 1, 128, new_payload );									// drive with alternate prototype

	msg->Set_len( 128 );
	msg->Set_mtype( 100 );
	msg->Set_subid( -10 );

	msg->Send_response( 100, new_p_ref );										// send a response (rts) with establisehd message type etc
	msg->Send_response( 100, new_payload );
	msg->Send_response( 100, 10, 100, new_p_ref );
	msg->Send_response( 100, 10, 100, new_payload );


	msg = NULL;								// should drive the message destroyer for coverage

	msg = x->Receive( 2000 );
	if( msg == NULL ) {
		fprintf( stderr, "<FAIL> expected message from receive but got nil\n" );
		errors++;
	}

	tinfo = new std::thread;				// start killer thread to terminate things so that run doesn't hang forever
	tinfo = new std::thread( killer, x );

	x->Run( nthreads );
	x->Halt();			// drive for coverage


	return errors > 0;
}
