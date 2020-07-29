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
	Mnemonic:	xapp_t3.cpp
	Abstract:	This is a simple xAPP which includes using the config support
				in the framework.  It is based on the xapp_t1 example and doesn't
				do much more than that.

	Date:		30 July 2020
	Author:		E. Scott Daniels

	Caution:	This example code is pulled directly into one or more of the documents
				(starting from the "start-example" tag below.  Use caution with
				line lengths and contents because of the requirement that this
				be documentation as well as working code.
*/
// start-example
#include <stdio.h>

#include "ricxfcpp/config.hpp"
#include "ricxfcpp/message.hpp"
#include "ricxfcpp/msg_component.hpp"
#include <ricxfcpp/metrics.hpp>
#include "ricxfcpp/xapp.hpp"

int vlevel = 0;					// verbose mode set via config

/*
	Just print something to the tty when we receive a message
	and are in verbose mode.
*/
void cb1( xapp::Message& mbuf, int mtype, int subid, int len,
			xapp::Msg_component payload,  void* data ) {
	if( vlevel > 0 ) {
		fprintf( stdout, "message received is %d bytes long\n", len );
	}
}

/*
	Driven when the configuration changes. We snarf the verbose
	level from the new config and update it. If it changed to
	>0, incoming messages should be recorded with a tty message.
	If set to 0, then tty output will be disabled.
*/
void config_cb( xapp::Config& c, void* data ) {
	int* vp;

	if( (vp = (int *) data) != NULL ) {
		*vp = c.Get_control_value( "verbose_level", *vp );
	}
}

int main( int argc, char** argv ) {
	Xapp*	x;
	int		nthreads = 1;
	std::unique_ptr<xapp::Config> cfg;

	// only parameter recognised is the config file name
	if( argc > 1 ) {
		cfg = std::unique_ptr<xapp::Config>( new xapp::Config( std::string( argv[1] ) ) );
	} else {
		cfg = std::unique_ptr<xapp::Config>( new xapp::Config( ) );
	}

	// must get a port from the config; no default
	auto port = cfg->Get_port( "rmr-data" );
	if( port.empty() ) {
		fprintf( stderr, "<FAIL> no port in config file\n" );
		exit( 1 );
	}

	// dig other data from the config
	vlevel = cfg->Get_control_value( "verbose_level", 0 );
	nthreads = cfg->Get_control_value( "thread_count", 1 );
	// very simple flag processing (no bounds/error checking)

	if( vlevel > 0 ) {
		fprintf( stderr, "<XAPP> listening on port: %s\n", port.c_str() );
		fprintf( stderr, "<XAPP> starting %d threads\n", nthreads );
	}

	// register the config change notification callback
	cfg->Set_callback( config_cb, (void *) &vlevel );

	x = new Xapp( port.c_str(), true );
	x->Add_msg_cb( 1, cb1, x );		// register message callback

	x->Run( nthreads );				// let framework drive
	// control should not return
}
