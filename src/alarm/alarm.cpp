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
	Mnemonic:	alarm.cpp
	Abstract:	This class provides an API to the alarm collector/reporter.
				An object is insanced by the user xAPP allowing the xAPP
				to send, clear, or resend the alarm as is necessary.


	Date:		15 July 2020
	Author:		E. Scott Daniels
*/

#include <string.h>
#include <unistd.h>
#include <time.h>

//#include <rmr/rmr.h>
#include <rmr/RIC_message_types.h>

#include <iostream>

#include "msg_component.hpp"
#include "message.hpp"
#include "alarm.hpp"

extern char* __progname;			// runtime lib supplied since we don't get argv[0]

namespace xapp {



// ------ private ----------------------------------------------

/*
	Suss out the alarm target from environment.

	We expect two variables in the environment:
		ALARM_MGR_SERVICE_NAME
		ALARM_MGR_SERVICE_PORT

	If name is not given, localhost is assumed. If port is not given
	then we assume 4560 (the defacto RMR listen port).
*/
static std::string endpoint_addr( ) {
	char*	et;									// environment token
	std::string addr = "localhost";
	std::string port = "4560";

	if( (et = getenv( "ALARM_MGR_SERVICE_NAME" )) != NULL ) {
		addr = std::string( et );
	}

	if( (et = getenv( "ALARM_MGR_SERVICE_PORT" )) != NULL ) {
		port = std::string( et );
	}

	return addr + ":" + port;
}


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
	Build the alarm json message with the current data.
	Returns the length of the payload inserted.
*/
int xapp::Alarm::build_alarm( int action_id, xapp::Msg_component payload, int payload_len ) {
	//char	wbuf[4096];
	std::string action;
	int used;

	if( app_id.compare( "" ) == 0 ) {
		app_id = std::string( __progname );				// see comment for extern above
	}

	if( severity.compare( "" ) == 0 ) {
		Set_severity( Alarm::SEV_WARN );
	}

	switch( action_id ) {
		case	Alarm::ACT_CLEAR:
			action = "CLEAR";
			break;

		case	Alarm::ACT_CLEAR_ALL:
			action = "CLEARALL";
			break;

		default:
			action = "RAISE";
			break;
	}

	//memset( wbuf, 0, sizeof( wbuf ) );
	//snprintf( wbuf, sizeof( wbuf ),
	used = snprintf( (char *) payload.get(), payload_len,
			"{  "
			"\"managedObjectId\": \"%s\", "
			"\"applicationId\": \"%s\", "
			"\"specificProblem\": %d, "
			"\"perceivedSeverity\": \"%s\", "
			"\"identifyingInfo\": \"%s\", "
			"\"additionalInfo\": \"%s\", "
			"\"AlarmAction\": \"%s\", "
			"\"AlarmTime\": %lld"
			" }",

			me_id.c_str(),
			app_id.c_str(),
			problem_id,
			severity.c_str(),
			info.c_str(),
			add_info.c_str(),
			action.c_str(),
			now()
	);

	//action = std::string( wbuf );
	return used;
}

// --------------- builders/operators  -------------------------------------

/*
	Create a new message wrapper for an existing RMR msg buffer.

	msg is a message which was allocaed by the framework and thus has the
	mrc reference embedded.
*/
xapp::Alarm::Alarm( std::shared_ptr<Message> msg ) :
	msg( msg ),
	endpoint( endpoint_addr() ),
	whid( -1 ),
	app_id( "" ),
	me_id( "" ),
	problem_id( -1 ),
	info( "" ),
	add_info( "" ),
	action( "" )
{ /* empty body */ }

/*
	Parameterised constructor (avoids calling setters after creation).
*/
xapp::Alarm::Alarm( std::shared_ptr<Message> msg, int prob_id, std::string meid  ) :
	msg( msg ),
	endpoint( endpoint_addr() ),
	whid( -1 ),
	app_id( "" ),
	me_id( meid ),
	problem_id( prob_id ),
	info( "" ),
	add_info( "" ),
	action( "" )
{ /* empty body */ }

xapp::Alarm::Alarm( std::shared_ptr<Message> msg, std::string meid  ) :
	msg( msg ),
	endpoint( endpoint_addr() ),
	whid( -1 ),
	app_id( "" ),
	me_id( meid ),
	problem_id( -1 ),
	info( "" ),
	add_info( "" ),
	action( "" )
{ /* empty body */ }



// ------------------ copy and move support ---------------------------------

/*
	Copy builder.  Given a source object instance (soi), create a copy.
	Creating a copy should be avoided as it can be SLOW!
*/
xapp::Alarm::Alarm( const Alarm& soi ) {
	msg = soi.msg;
	endpoint = soi.endpoint;
	whid = soi.whid;

	me_id = soi.me_id;				// user stuff
	app_id = soi.app_id;
	problem_id = soi.problem_id;
	severity = soi.severity;
	info = soi.info;
	add_info = soi.add_info;
}

/*
	Assignment operator. Simiolar to the copycat, but "this" object exists and
	may have data that needs to be released prior to making a copy of the soi.
*/
Alarm& xapp::Alarm::operator=( const Alarm& soi ) {
	if( this != &soi ) {				// cannot do self assignment
		msg = soi.msg;
		endpoint = soi.endpoint;
		whid = soi.whid;

		me_id = soi.me_id;
		app_id = soi.app_id;
		problem_id = soi.problem_id;
		severity = soi.severity;
		info = soi.info;
		add_info = soi.add_info;
	}

	return *this;
}

/*
	Move builder.  Given a source object instance (soi), move the information from
	the soi ensuring that the destriction of the soi doesn't trash things from
	under us.
*/
xapp::Alarm::Alarm( Alarm&& soi ) {
	msg = soi.msg;		// capture pointers and copy data before setting soruce things to nil
	endpoint = soi.endpoint;
	whid = soi.whid;

	me_id = soi.me_id;
	app_id = soi.app_id;
	problem_id = soi.problem_id;
	severity = soi.severity;
	info = soi.info;
	add_info = soi.add_info;

	soi.msg = NULL;		// prevent closing of RMR stuff on soi destroy
}

/*
	Move Assignment operator. Move the message data to the existing object
	ensure the object reference is cleaned up, and ensuring that the source
	object references are removed.
*/
Alarm& xapp::Alarm::operator=( Alarm&& soi ) {
	if( this != &soi ) {				// cannot do self assignment
		// anything that needs to be freed/delted from soi, must be done here

		msg = soi.msg;			// move pointers and values
		endpoint = soi.endpoint;
		whid = soi.whid;

		me_id = soi.me_id;
		app_id = soi.app_id;
		problem_id = soi.problem_id;
		severity = soi.severity;
		info = soi.info;
		add_info = soi.add_info;

		soi.msg = NULL;			// prevent bad things when source is destroyed
	}

	return *this;
}

/*
	Destroyer.
*/
xapp::Alarm::~Alarm() {

	msg = NULL;
}


// ---- setters -------------------------------------------------

void xapp::Alarm::Set_meid( std::string new_meid ) {
	me_id = new_meid;
}

void xapp::Alarm::Set_severity( int new_sev ) {
	switch( new_sev ) {
		case	Alarm::SEV_CRIT:
			severity = "CRITICAL";
			break;

		case	Alarm::SEV_MAJOR:
			severity = "MAJOR";
			break;

		case	Alarm::SEV_MINOR:
			severity = "MINOR";
			break;

		case	Alarm::SEV_WARN:
			severity = "WARNING";
			break;

		case	Alarm::SEV_CLEAR:
			severity = "CLEARED";
			break;

		default:
			severity = "DEFAULT";
			break;
	}
}

void xapp::Alarm::Set_appid( std::string new_id ) {
	app_id = new_id;
}

void xapp::Alarm::Set_problem( int new_id ) {
	problem_id = new_id;
}

void xapp::Alarm::Set_info( std::string new_info ) {
	info = new_info;
}

void xapp::Alarm::Set_additional( std::string new_info ) {
	add_info = new_info;
}

void xapp::Alarm::Set_whid( int new_whid ) {
	whid = new_whid;
}

void xapp::Alarm::Dump() {
	fprintf( stderr, "Alarm: prob id: %d\n", problem_id );
	fprintf( stderr, "Alarm: meid: %s\n", me_id.c_str() );
	fprintf( stderr, "Alarm: app: %s\n", app_id.c_str() );
	fprintf( stderr, "Alarm: info: %s\n", info.c_str() );
	fprintf( stderr, "Alarm: ainfo: %s\n", add_info.c_str() );
}

// ------------------- getters ------------------------------------

/*
	Return the enpoint address string we have.
*/
std::string xapp::Alarm::Get_endpoint( ) {
	return endpoint;
}

// ------- message sending ---------------------------------------

/*
	Send a raise message with the alarm contents unchanged.
*/
bool xapp::Alarm::Raise( ) {
	int used;
	used = build_alarm( ACT_RAISE, msg->Get_payload(), msg->Get_available_size() );
	msg->Wormhole_send( whid,  RIC_ALARM, xapp::Message::NO_SUBID, used + 1, NULL );
}

/*
	Additional prototypes allow for avoiding some setter calls when raising alarms.
	Severity is one of our SEV_* constants. Problem is the caller's assigned
	problem ID. Info and addional_info are user supplied data that is just passed
	through.
*/
bool xapp::Alarm::Raise( int severity, int problem, std::string info ) {
	this->severity = severity;
	problem_id = problem;
	this->info = info;

	Raise();
}

bool xapp::Alarm::Raise( int severity, int problem, std::string info, std::string additional_info ) {
	this->severity = severity;
	problem_id = problem;
	this->info = info;
	this->add_info = additional_info;

	Raise();
}

/*
	Send a clear message with the contents of the alarm otherwise unchanged.
*/
bool xapp::Alarm::Clear( ) {
	int used;

	used = build_alarm( ACT_CLEAR, msg->Get_payload(), msg->Get_available_size() );
	msg->Wormhole_send( whid,  RIC_ALARM, xapp::Message::NO_SUBID, used + 1, NULL );
}

/*
	Additional prototypes allow for avoiding some setter calls when raising alarms.
	Severity is one of our SEV_* constants. Problem is the caller's assigned
	problem ID. Info and addional_info are user supplied data that is just passed
	through.
*/
bool xapp::Alarm::Clear( int severity, int problem, std::string info ) {
	this->severity = severity;
	problem_id = problem;
	this->info = info;

	Clear();
}

bool xapp::Alarm::Clear( int severity, int problem, std::string info, std::string additional_info ) {
	this->severity = severity;
	problem_id = problem;
	this->info = info;
	this->add_info = additional_info;

	Clear();
}


/*
	Send a clear-all message. The contents of the alarm are unaffected.
*/
bool xapp::Alarm::Clear_all( ) {
	int used;

	used = build_alarm( ACT_CLEAR_ALL, msg->Get_payload(), msg->Get_available_size() );
	msg->Wormhole_send( whid,  RIC_ALARM, xapp::Message::NO_SUBID, used + 1, NULL );
}


/*
	This is a convenience function which sends a clear message followed by a
	raise message. Alarm contents are not adjusted.
*/
bool xapp::Alarm::Raise_again( ) {
	Clear();
	Raise();
}



} // namespace
