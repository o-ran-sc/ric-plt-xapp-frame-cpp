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
	Mnemonic:	rmr_dump.cpp
	Abstract:	This is a sample xapp which will behave somewhat like tcpdump
				as it receives rmr messages.  The list of message types which should
				be processed are given on the command line, following any flag
				options (e.g. -p port).  Any of those messages received are reported
				on by the application.  The verbosity level may be used to increase
				the amount of detail given for the tracked messages.

				This writes to the TTY which is slow, so do not expect it to be able
				to process and report on a high rate of messages. Also, forwarded
				messages will reach the intended target, however if the target
				attempts to send a response the response will come back to THIS
				application, and not the message origination; this cannot be a bit
				of middleware in it's current form.

	Date:		25 March 2020
	Author:		E. Scott Daniels

	Caution:	This example code is pulled directly into one or more of the documents
				(starting from the "start-example" tag below.  Use caution with
				line lengths and contents because of the requirement that this
				be documentation as well as working code.
*/
// start-example
#include <stdio.h>
#include <unistd.h>
#include <atomic>

#include "ricxfcpp/xapp.hpp"

/*
	Information that the callback needs outside
	of what is given to it via parms on a call
	by the framework.
*/
typedef struct {
	int		vlevel;             // verbosity level
	bool	forward;            // if true, message is forwarded
	int		stats_freq;         // header/stats after n messages
	std::atomic<long>	pcount; // messages processed
	std::atomic<long>	icount; // messages ignored
	std::atomic<int>	hdr;    // number of messages before next header
} cb_info_t;

// ----------------------------------------------------------------------

/*
	Dump bytes to tty.
*/
void dump( unsigned const char* buf, int len ) {
	int		i;
	int		j;
	char	cheater[17];

	fprintf( stdout, "<RD> 0000 | " );
	j = 0;
	for( i = 0; i < len; i++ ) {
		cheater[j++] =  isprint( buf[i] ) ? buf[i] : '.';
		fprintf( stdout, "%02x ", buf[i] );

		if( j == 16 ) {
			cheater[j] = 0;
			fprintf( stdout, " | %s\n<RD> %04x | ", cheater, i+1 );
			j = 0;
		}
	}

	if( j ) {
		i = 16 - (i % 16);
		for( ; i > 0; i-- ) {
			fprintf( stdout, "   " );
		}
		cheater[j] = 0;
		fprintf( stdout, " | %s\n", cheater );
	}
}

/*
	generate stats when the hdr count reaches 0. Only one active
	thread will ever see it be exactly 0, so this is thread safe.
*/
void stats( cb_info_t& cbi ) {
	int curv;					// current stat trigger value

	curv = cbi.hdr--;

	if( curv == 0 ) {					// stats when we reach 0
		fprintf( stdout, "ignored: %ld  processed: %ld\n",
			cbi.icount.load(), cbi.pcount.load() );
		if( cbi.vlevel > 0 ) {
			fprintf( stdout, "\n     %5s %5s %2s %5s\n",
				"MTYPE", "SUBID", "ST", "PLLEN" );
		}

		cbi.hdr = cbi.stats_freq;		// reset must be last
	}
}

void cb1( xapp::Message& mbuf, int mtype, int subid, int len,
				xapp::Msg_component payload,  void* data ) {
	cb_info_t*	cbi;
	long total_count;

	if( (cbi = (cb_info_t *) data) == NULL ) {
		return;
	}

	cbi->pcount++;
	stats( *cbi );			// gen stats & header if needed

	if( cbi->vlevel > 0 ) {
		fprintf( stdout, "<RD> %-5d %-5d %02d %-5d \n",
				mtype, subid, mbuf.Get_state(), len );

		if( cbi->vlevel > 1 ) {
			dump(  payload.get(), len > 64 ? 64 : len );
		}
	}

	if( cbi->forward ) {
		// forward with no change to len or payload
		mbuf.Send_msg( xapp::Message::NO_CHANGE, NULL );
	}
}

/*
	registered as the default callback; it counts the
	messages that we aren't giving details about.
*/
void cbd( xapp::Message& mbuf, int mtype, int subid, int len,
				xapp::Msg_component payload,  void* data ) {
	cb_info_t*	cbi;

	if( (cbi = (cb_info_t *) data) == NULL ) {
		return;
	}

	cbi->icount++;
	stats( *cbi );

	if( cbi->forward ) {
		// forward with no change to len or payload
		mbuf.Send_msg( xapp::Message::NO_CHANGE, NULL );
	}
}

int main( int argc, char** argv ) {
	std::unique_ptr<Xapp> x;
	char*	port = (char *) "4560";
	int ai = 1;					// arg processing index
	cb_info_t*	cbi;
	int		ncb = 0;			// number of callbacks registered
	int		mtype;
	int		nthreads = 1;

	cbi = (cb_info_t *) malloc( sizeof( *cbi ) );
	cbi->pcount = 0;
	cbi->icount = 0;
	cbi->stats_freq = 10;

	ai = 1;
	// very simple flag parsing (no error/bounds checking)
	while( ai < argc ) {
		if( argv[ai][0] != '-' )  {		// break on first non-flag
			break;
		}

		// very simple arg parsing; each must be separate -x -y not -xy.
		switch( argv[ai][1] ) {
			case 'f':					// enable packet forwarding
				cbi->forward = true;
				break;

			case 'p':					// define port
				port = argv[ai+1];
				ai++;
				break;

			case 's':						// stats frequency
				cbi->stats_freq = atoi( argv[ai+1] );
				if( cbi->stats_freq < 5 ) {	// enforce sanity
					cbi->stats_freq = 5;
				}
				ai++;
				break;

			case 't':						// thread count
				nthreads = atoi( argv[ai+1] );
				if( nthreads < 1 ) {
					nthreads = 1;
				}
				ai++;
				break;

			case 'v':			// simple verbose bump
				cbi->vlevel++;
				break;

			case 'V':			// explicit verbose level
				cbi->vlevel = atoi( argv[ai+1] );
				ai++;
				break;

			default:
				fprintf( stderr, "unrecognised option: %s\n", argv[ai] );
				fprintf( stderr, "usage: %s [-f] [-p port] "
								"[-s stats-freq]  [-t thread-count] "
								"[-v | -V n] msg-type1 ... msg-typen\n",
								argv[0] );
				fprintf( stderr, "\tstats frequency is based on # of messages received\n" );
				fprintf( stderr, "\tverbose levels (-V) 0 counts only, "
								"1 message info 2 payload dump\n" );
				exit( 1 );
		}

		ai++;
	}

	cbi->hdr = cbi->stats_freq;
	fprintf( stderr, "<RD> listening on port: %s\n", port );

	// create xapp, wait for route table if forwarding
	x = std::unique_ptr<Xapp>( new Xapp( port, cbi->forward ) );

	// register callback for each type on the command line
	while( ai < argc ) {
		mtype = atoi( argv[ai] );
		ai++;
		fprintf( stderr, "<RD> capturing messages for type %d\n", mtype );
		x->Add_msg_cb( mtype, cb1, cbi );
		ncb++;
	}

	if( ncb < 1 ) {
		fprintf( stderr, "<RD> no message types specified on the command line\n" );
		exit( 1 );
	}

	x->Add_msg_cb( x->DEFAULT_CALLBACK, cbd, cbi );		// register default cb

	fprintf( stderr, "<RD> starting driver\n" );
	x->Run( nthreads );

	// return from run() is not expected, but some compilers might
	// compilain if there isn't a return value here.
	return 0;
}
