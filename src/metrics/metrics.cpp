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
	Mnemonic:	metrics.cpp
	Abstract:	Impementation of the metrics functions. (see metrics.hpp)


		void Set_source( std::string new_source );
		void Set_reporter( std::string new_reporter );
		void Push_data( std::string key, double value );

	Date:		20 July 2020
	Author:		E. Scott Daniels
*/

#include <string.h>
#include <unistd.h>
#include <time.h>

#include <rmr/RIC_message_types.h>
#ifndef RIC_METRICS
	#define RIC_METRICS 120
#endif

#include <iostream>

#include "msg_component.hpp"
#include "message.hpp"
#include "metrics.hpp"

extern char* __progname;			// runtime lib supplied since we don't get argv[0]



namespace xapp {



// ------ private ----------------------------------------------


/*
	Return the current time in milliseconds past the UNIX epoch.
*/
static long long now( void ) {
	struct timespec ts;
	long long       now = 0;

	clock_gettime( CLOCK_REALTIME, &ts );
	now = (ts.tv_sec * 1000000) + (ts.tv_nsec/1000000);	// convert nano to milli and bang onto seconds

	return now;
}


/*
	Build the payload that we'll send.
	Accepts a pointer to the payload message component in the RMR message that we are
	to fill in, along with the maxmimum length of the payload.  Returns the length of
	the payload that was acutally used.

	Currently, the munchkin expects:
		{
			"reporter": "<string>",
			"generator" "<string>",
			"timestamp": double,
			"data": [
				{
					"id": "<string>",
					"type": "<string>",
					"value": double
				},...
			]
		}
*/
int xapp::Metrics::build_payload( xapp::Msg_component payload, int max_len ) {
	int used;											// actual size of payload created
	std::string generator;


	if(  data.compare( "" ) == 0 ) {					// xAPP never pushed any data
		return 0;										// don't build, just bail
	}

	generator = source.compare( "" ) == 0 ? reporter : source;

	used = snprintf( (char *) payload.get(), max_len,
			"{  "
			"\"reporter\": \"%s\", "
			"\"generator\": \"%s\", "
			"\"timestamp\": %lld, "
			"\"data\": [ %s ] "
			" }",
			reporter.c_str(),
			generator.c_str(),
			now(),
			data.c_str()
	);

	return used;
}


// --------------- builders/operators  -------------------------------------

/*
	Create a new metrics "message" with a provided (empty) RMR message.
*/
xapp::Metrics::Metrics( std::shared_ptr<Message> msg ) :
	msg( msg ),
	reporter( __progname ),
	source( "" ),
	data( "" )
{ /* empty body */ }

/*
	Build a metrics for a value source other than the calling xAPP
*/
xapp::Metrics::Metrics( std::shared_ptr<Message> msg, std::string msource ) :
	msg( msg ),
	reporter( __progname ),
	source( msource ),
	data( "" )
{ /* empty body */ }

/*
	Build a metrics object that allows the xAPP to set it's reporter name
	rather than assuming the programme name and the source.

	The xAPP can pass "" as the msource if the intent is just to supply
	an alternate programme name which is also the source.
*/
xapp::Metrics::Metrics( std::shared_ptr<Message> msg, std::string reporter, std::string msource ) :
	msg( msg ),
	reporter( reporter ),
	source( msource ),
	data( "" )
{ /* empty body */ }

// ------------------ copy and move support ---------------------------------

/*
	Copy builder.  Given a source object instance (soi), create a copy.
	Creating a copy should be avoided as it can be SLOW!
*/
xapp::Metrics::Metrics( const Metrics& soi ) {
	msg = soi.msg;
	data = soi.data;
	source = soi.source;
	reporter = soi.reporter;
}

/*
	Assignment operator. Simiolar to the copycat, but "this" object exists and
	may have data that needs to be released prior to making a copy of the soi.
*/
Metrics& xapp::Metrics::operator=( const Metrics& soi ) {
	if( this != &soi ) {				// cannot do self assignment
		// anything that must be freed from 'this' must be done here

		msg = soi.msg;
		data = soi.data;
		source = soi.source;
		reporter = soi.reporter;
	}

	return *this;
}

/*
	Move builder.  Given a source object instance (soi), move the information from
	the soi ensuring that the destriction of the soi doesn't trash things from
	under us.
*/
xapp::Metrics::Metrics( Metrics&& soi ) {
	msg = soi.msg;		// capture pointers and copy data before setting soruce things to nil
	data = soi.data;
	source = soi.source;
	reporter = soi.reporter;

	soi.msg = NULL;		// prevent closing of RMR stuff on soi destroy
}

/*
	Move Assignment operator. Move the message data to the existing object
	ensure the object reference is cleaned up, and ensuring that the source
	object references are removed.
*/
Metrics& xapp::Metrics::operator=( Metrics&& soi ) {
	if( this != &soi ) {				// cannot do self assignment
		// anything that needs to be freed/delted from 'this', must be done here

		msg = soi.msg;			// move pointers and values
		data = soi.data;
		source = soi.source;
		reporter = soi.reporter;

		soi.msg = NULL;			// prevent bad things when source is destroyed
	}

	return *this;
}

/*
	Destroyer.
*/
xapp::Metrics::~Metrics() {
	msg = NULL;
}


// ---- setters -------------------------------------------------

void xapp::Metrics::Set_source( std::string new_source ) {
	source = new_source;
}

void xapp::Metrics::Set_reporter( std::string new_reporter ) {
	reporter = new_reporter;
}

/*
	Pushes the key/value pair onto the current data list.
	This could be more efficent, but for now it is simple.
*/
void xapp::Metrics::Push_data( std::string key, double value ) {
	char	wbuf[1024];
	char*	sep = (char *) " ";

	if( data.compare( "" ) != 0 ) {			// first on the list, no need for preceeding comma
		sep = (char *) ", ";
	}

	snprintf( wbuf, sizeof( wbuf ), "%s{ \"id\": \"%s\", \"value\": %.5f }", sep, key.c_str(), value );

	data += std::string( wbuf );
}

// ------------------- getters ------------------------------------


// ------- message sending ---------------------------------------

/*
	Send the message by building the payload and passing to RMR.
	Returns the state of the send (true == good). If the xapp did not
	push any data, no send is actually invoked, and true is reported.
*/
bool xapp::Metrics::Send( ) {
	int		used = 0;			// actual num bytes used in the payload
	bool	state = true;

	used = build_payload( msg->Get_payload(), msg->Get_available_size() - 1 );
	if( used > 0 ) {
		//fprintf( stderr, ">>>> sending payload: %s\n", (char *) msg->Get_payload().get() );
		state = msg->Send_msg(  RIC_METRICS, RMR_VOID_SUBID, used+1, NULL );
	}

	data = "";				// clear data after the send

	return state;
}



} // namespace
