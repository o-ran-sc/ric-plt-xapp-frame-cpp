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
	Mnemonic:	message.cpp
	Abstract:	A message wrapper. This should completely hide the
				underlying transport (RMR) message structure from
				the user application. For the most part, the getters
				are used by the framwork; it is unlikely that other
				than adding/extracting the MEID, the user app will
				be completely unaware of information that is not
				presented in the callback parms.

	Date:		12 March 2020
	Author:		E. Scott Daniels
*/

#include <string.h>
#include <unistd.h>

#include <rmr/rmr.h>

#include <iostream>

#include "message.hpp"

namespace xapp {



// --------------- private ------------------------------------------------

// --------------- builders/operators  -------------------------------------

/*
	Create a new message wrapper for an existing RMR msg buffer.
*/
xapp::Message::Message( rmr_mbuf_t* mbuf, void* mrc ) :
	mrc(  mrc ),			// the message router context for sends
	mbuf(  mbuf )
{  /* empty body */ }

xapp::Message::Message( void* mrctx, int payload_len ) :
	mrc(  mrctx ),										// the message router context for sends
	mbuf(  rmr_alloc_msg( mrc, payload_len ) )
{ /* empty body */ }
//	this->mrc = mrc;
	//this->mbuf = rmr_alloc_msg( mrc, payload_len );

/*
	Copy builder.  Given a source object instance (soi), create a copy.
	Creating a copy should be avoided as it can be SLOW!
*/
xapp::Message::Message( const Message& soi ) :
	mrc( soi.mrc )
{
	int payload_size;

	payload_size = rmr_payload_size( soi.mbuf );		// rmr can handle a nil pointer
	mbuf = rmr_realloc_payload( soi.mbuf, payload_size, RMR_COPY, RMR_CLONE );
}

/*
	Assignment operator. Simiolar to the copycat, but "this" object exists and
	may have data that needs to be released prior to making a copy of the soi.
*/
Message& xapp::Message::operator=( const Message& soi ) {
	int	payload_size;

	if( this != &soi ) {				// cannot do self assignment
		if( mbuf != NULL ) {
			rmr_free_msg( mbuf );		// release the old one so we don't leak
		}

		payload_size = rmr_payload_size( soi.mbuf );		// rmr can handle a nil pointer
		mrc = soi.mrc;
		mbuf = rmr_realloc_payload( soi.mbuf, payload_size, RMR_COPY, RMR_CLONE );
	}

	return *this;
}

/*
	Move builder.  Given a source object instance (soi), move the information from
	the soi ensuring that the destriction of the soi doesn't trash things from
	under us.
*/
xapp::Message::Message( Message&& soi ) :
	mrc(  soi.mrc ),
	mbuf(  soi.mbuf )
{
	soi.mrc = NULL;		// prevent closing of RMR stuff on soi destroy
	soi.mbuf = NULL;
}

/*
	Move Assignment operator. Move the message data to the existing object
	ensure the object reference is cleaned up, and ensuring that the source
	object references are removed.
*/
Message& xapp::Message::operator=( Message&& soi ) {
	if( this != &soi ) {				// cannot do self assignment
		if( mbuf != NULL ) {
			rmr_free_msg( mbuf );		// release the old one so we don't leak
		}

		mrc = soi.mrc;
		mbuf = soi.mbuf;

		soi.mrc = NULL;
		soi.mbuf = NULL;
	}

	return *this;
}


/*
	Destroyer.
*/
xapp::Message::~Message() {
	if( mbuf != NULL ) {
		rmr_free_msg( mbuf );
	}

	mbuf = NULL;
}


// --- getters/setters -----------------------------------------------------
/*
	Copy the payload bytes, and return a smart pointer (unique) to it.
	If the application needs to update the payload in place for a return
	to sender call, or just to access the payload in a more efficent manner
	(without the copy), the Get_payload() function should be considered.

	This function will return a NULL pointer if malloc fails.
*/
//char* Message::Copy_payload( ){
std::unique_ptr<unsigned char> xapp::Message::Copy_payload( ){

	if( mbuf != NULL ) {
		unsigned char*	new_payload = new unsigned char[mbuf->len];
		memcpy( new_payload, mbuf->payload, mbuf->len );
		return std::unique_ptr<unsigned char>( new_payload );
	}

	return NULL;
}

/*
	Makes a copy of the MEID and returns a smart pointer to it.
*/
std::unique_ptr<unsigned char> xapp::Message::Get_meid() const {
	unsigned char* m = NULL;

	m = (unsigned char *) malloc( sizeof( unsigned char ) * RMR_MAX_MEID );
	rmr_get_meid( mbuf, m );

	return std::unique_ptr<unsigned char>( m );
}

/*
	Return the total size of the payload (the amount that can be written to
	as opposed to the portion of the payload which is currently in use.
	If mbuf isn't valid (nil, or message has a broken header) the return
	will be -1.
*/
int xapp::Message::Get_available_size() const {
	return rmr_payload_size( mbuf );		// rmr can handle a nil pointer
}

int	xapp::Message::Get_mtype() const {
	int rval = INVALID_MTYPE;

	if( mbuf != NULL ) {
		rval = mbuf->mtype;
	}

	return rval;
}

/*
	Makes a copy of the source field and returns a smart pointer to it.
*/
std::unique_ptr<unsigned char> xapp::Message::Get_src() const {
	unsigned char* m = new unsigned char[RMR_MAX_SRC];

	if( m != NULL ) {
		rmr_get_src( mbuf, m );
	}

	return std::unique_ptr<unsigned char>( m );
}

int	xapp::Message::Get_state( ) const {
	int state = INVALID_STATUS;

	if( mbuf != NULL ) {
		state =  mbuf->state;
	}

	return state;
}

int	xapp::Message::Get_subid() const {
	int	rval = INVALID_SUBID;

	if( mbuf != NULL ) {
		rval =mbuf->sub_id;
	}

	return rval;
}

/*
	Return the amount of the payload (bytes) which is used. See
	Get_available_size() to get the total usable space in the payload.
*/
int	xapp::Message::Get_len() const {
	int rval = 0;

	if( mbuf != NULL ) {
		rval = mbuf->len;
	}

	return rval;
}

/*
	This returns a smart (unique) pointer to the payload portion of the
	message. This provides the user application with the means to
	update the payload in place to avoid multiple copies.  The
	user programme is responsible to determing the usable payload
	length by calling Message:Get_available_size(), and ensuring that
	writing beyond the indicated size does not happen.
*/
Msg_component xapp::Message::Get_payload() const {
	if( mbuf != NULL ) {
		return std::unique_ptr<unsigned char, unfreeable>( mbuf->payload );
	}

	return NULL;
}

void xapp::Message::Set_meid( std::shared_ptr<unsigned char> new_meid ) {
	if( mbuf != NULL ) {
		rmr_str2meid( mbuf, new_meid.get() );
	}
}

void xapp::Message::Set_mtype( int new_type ){
	if( mbuf != NULL ) {
		mbuf->mtype = new_type;
	}
}

void xapp::Message::Set_len( int new_len ){
	if( mbuf != NULL  && new_len >= 0 ) {
		mbuf->len = new_len;
	}
}

void xapp::Message::Set_subid( int new_subid ){
	if( mbuf != NULL ) {
		mbuf->sub_id = new_subid;
	}
}


// -------------- send functions ---------------------------------

/*
	This assumes that the contents of the mbuf were set by either a send attempt that
	failed with a retry and thus is ready to be processed by RMR.
	Exposed to the user, but not expected to be frequently used.
*/
bool xapp::Message::Send( ) {
	bool state = false;

	if( mbuf != NULL ) {
		mbuf =  rmr_send_msg( mrc, mbuf );	// send and pick up new mbuf
		state = mbuf->state == RMR_OK;		// overall state for caller
	}

	return state;
}

/*
	Similar to Send(), this assumes that the message is already set up and this is a retry.
	Exposed to the user, but not expected to be frequently used.
*/
bool xapp::Message::Reply( ) {
	bool state = false;

	if( mbuf != NULL ) {
		mbuf =  rmr_rts_msg( mrc, mbuf );		// send and pick up new mbuf
		state = mbuf->state == RMR_OK;			// state for caller based on send
	}

	return state;
}

/*
	Send workhorse.
	This will setup the message (type etc.) ensure the message payload space is
	large enough and copy in the payload (if a new payload is given), then will
	either send or rts the message based on the stype parm.

	If payload is nil, then we assume the user updated the payload in place and
	no copy is needed.

	This is public, but most users should use Send_msg or Send_response functions.
*/
bool xapp::Message::Send( int mtype, int subid, int payload_len, unsigned char* payload, int stype, rmr_whid_t whid ) {
	bool state = false;

	if( mbuf != NULL ) {
		if( mtype != NO_CHANGE ) {
			mbuf->mtype = mtype;
		}
		if( subid != NO_CHANGE ) {
			mbuf->sub_id = subid;
		}

		if( payload_len != NO_CHANGE ) {
			mbuf->len = payload_len;
		}

		if( payload != NULL ) {			// if we have a payload, ensure msg has room, realloc if needed, then copy
			mbuf = rmr_realloc_payload( mbuf, payload_len, RMR_NO_COPY, RMR_NO_CLONE );		// ensure message is large enough
			if( mbuf == NULL ) {
				return false;
			}

			memcpy( mbuf->payload, payload, mbuf->len );
		}

		switch( stype ) {
			case RESPONSE:
				mbuf = rmr_rts_msg( mrc, mbuf );
				break;

			case MESSAGE:
				mbuf = rmr_send_msg( mrc, mbuf );
				break;

			case WORMHOLE_MSG:
				mbuf = rmr_wh_send_msg( mrc, whid,  mbuf );
				break;

			default:
				break;					// because sonar doesn't like defaultless switches even when there is no work :(
		}

		state = mbuf->state == RMR_OK;
	}

	return state;
}

/*
	Send a response to the endpoint that sent the original message.

	Response can be null and the assumption will be that the message payload
	was updated in place and no additional copy is needed before sending the message.

	The second form of the call allows for a stack allocated buffer (e.g. char foo[120]) to
	be passed as the payload.
*/
bool xapp::Message::Send_response(  int mtype, int subid, int response_len, std::shared_ptr<unsigned char> response ) {
	return Send( mtype, subid, response_len, response.get(), RESPONSE, NO_WHID );
}

bool xapp::Message::Send_response(  int mtype, int subid, int response_len, unsigned char* response ) {
	return Send( mtype, subid, response_len, response, RESPONSE, NO_WHID );
}

/*
	These allow a response message to be sent without changing the mtype/subid.
*/
bool xapp::Message::Send_response(  int response_len, std::shared_ptr<unsigned char> response ) {
	return Send( NO_CHANGE, NO_CHANGE, response_len, response.get(), RESPONSE, NO_WHID );
}

bool xapp::Message::Send_response(  int response_len, unsigned char* response ) {
	return Send( NO_CHANGE, NO_CHANGE, response_len, response, RESPONSE, NO_WHID );
}


/*
	Send a message based on message type routing.

	Payload can be null and the assumption will be that the message payload
	was updated in place and no additional copy is needed before sending the message.

	Return is a new mbuf suitable for sending another message, or the original buffer with
	a bad state sent if there was a failure.
*/
bool xapp::Message::Send_msg(  int mtype, int subid, int payload_len, std::shared_ptr<unsigned char> payload ) {
	return Send( mtype, subid, payload_len, payload.get(), MESSAGE, NO_WHID );
}

bool xapp::Message::Send_msg(  int mtype, int subid, int payload_len, unsigned char* payload ) {
	return Send( mtype, subid, payload_len, payload, MESSAGE, NO_WHID );
}

/*
	Similar send functions that allow the message type/subid to remain unchanged
*/
bool xapp::Message::Send_msg(  int payload_len, std::shared_ptr<unsigned char> payload ) {
	return Send( NO_CHANGE, NO_CHANGE, payload_len, payload.get(), MESSAGE, NO_WHID );
}

bool xapp::Message::Send_msg(  int payload_len, unsigned char* payload ) {
	return Send( NO_CHANGE, NO_CHANGE, payload_len, payload, MESSAGE, NO_WHID );
}


/*
	Wormhole send allows an xAPP to send a message directly based on an existing
	wormhole ID (use xapp::Wormhole_open() to get one). Wormholes should NOT be
	used for reponse messages, but are intended for things like route tables and
	alarm messages where routing doesn't exist/apply.
*/
bool xapp::Message::Wormhole_send( int whid, int mtype, int subid, int payload_len, std::shared_ptr<unsigned char> payload ) {
	return Send( mtype, subid, payload_len, payload.get(), WORMHOLE_MSG, whid );
}


} // namespace
