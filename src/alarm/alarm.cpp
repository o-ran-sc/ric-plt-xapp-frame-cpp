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
#include <iostream>

#include <rmr/RIC_message_types.h>

#include "msg_component.hpp"
#include "message.hpp"
#include "alarm.hpp"

#ifndef RIC_ALARM
	// this _should_ come from the message types header, but if not ensure we have something
	constexpr int ric_alarm_value = 110;
	#define RIC_ALARM ric_alarm_value
#endif

extern const char* __progname;			// runtime lib supplied since we don't get argv[0]

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
	const char*	et;									// environment token
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
	std::string maction;				// message action is a text string
	int used;

	if( app_id.compare( "" ) == 0 ) {
		app_id = std::string( __progname );				// see comment for extern above
	}

	if( severity.compare( "" ) == 0 ) {
		Set_severity( Alarm::SEV_WARN );
	}

	switch( action_id ) {
		case	Alarm::ACT_CLEAR:
			maction = "CLEAR";
			break;

		case	Alarm::ACT_CLEAR_ALL:
			maction = "CLEARALL";
			break;

		default:
			maction = "RAISE";
			break;
	}

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
			maction.c_str(),
			now()
	);

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
	endpoint( endpoint_addr() )
{ /* empty body */ }

/*
	Parameterised constructor (avoids calling setters after creation).
*/
xapp::Alarm::Alarm( std::shared_ptr<Message> msg, int prob_id, const std::string& meid  ) :
	msg( msg ),
	endpoint( endpoint_addr() ),
	me_id( meid ),
	problem_id( prob_id )
{ /* empty body */ }

xapp::Alarm::Alarm( std::shared_ptr<Message> msg, const std::string& meid  ) :
	msg( msg ),
	endpoint( endpoint_addr() ),
	me_id( meid )
{ /* empty body */ }



// ------------------ copy and move support ---------------------------------

/*
	Copy builder.  Given a source object instance (soi), create a copy.
	Creating a copy should be avoided as it can be SLOW!
*/
xapp::Alarm::Alarm( const Alarm& soi ) :
	msg(  soi.msg ),
	endpoint(  soi.endpoint ),
	whid(  soi.whid ),

	me_id(  soi.me_id ),				// user stuff
	app_id(  soi.app_id ),
	problem_id(  soi.problem_id ),
	severity(  soi.severity ),
	info(  soi.info ),
	add_info(  soi.add_info )
{ /* empty body */ }

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
xapp::Alarm::Alarm( Alarm&& soi )  :
	msg(  soi.msg ),					// order must match .hpp else sonar complains
	endpoint(  soi.endpoint ),
	whid(  soi.whid ),
	me_id(  soi.me_id ),
	app_id(  soi.app_id ),
	problem_id(  soi.problem_id ),
	severity(  soi.severity ),
	info(  soi.info ),
	add_info(  soi.add_info )
{

	soi.msg = NULL;		// prevent closing of RMR stuff on soi destroy
}

/*
	Move Assignment operator. Move the message data to the existing object
	ensure the object reference is cleaned up, and ensuring that the source
	object references are removed.
*/
Alarm& xapp::Alarm::operator=( Alarm&& soi ) noexcept {
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

void xapp::Alarm::Set_meid( const std::string& new_meid ) {
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

void xapp::Alarm::Set_appid( const std::string& new_id ) {
	app_id = new_id;
}

void xapp::Alarm::Set_problem( int new_id ) {
	problem_id = new_id;
}

void xapp::Alarm::Set_info( const std::string& new_info ) {
	info = new_info;
}

void xapp::Alarm::Set_additional( const std::string& new_info ) {
	add_info = new_info;
}

void xapp::Alarm::Set_whid( int new_whid ) {
	whid = new_whid;
}

void xapp::Alarm::Dump() const {
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
std::string xapp::Alarm::Get_endpoint( ) const {
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
bool xapp::Alarm::Raise( int new_severity, int problem, const std::string& cinfo ) {
	Set_severity( new_severity );
	problem_id = problem;
	info = cinfo;

	Raise();
}

bool xapp::Alarm::Raise( int new_severity, int problem, const std::string& cinfo, const std::string& additional_info ) {
	Set_severity( new_severity );
	problem_id = problem;
	info = cinfo;
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
bool xapp::Alarm::Clear( int new_severity, int problem, const std::string& cinfo ) {
	Set_severity( new_severity );
	problem_id = problem;
	info = cinfo;

	Clear();
}

bool xapp::Alarm::Clear( int new_severity, int problem, const std::string& cinfo, const std::string& additional_info ) {
	Set_severity( new_severity );
	problem_id = problem;
	info = cinfo;
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
