// vi: ts=4 sw=4 noet:
/*
==================================================================================
    Copyright (c) 2020 AT&T Intellectual Property.
    Copyright (c) 2020 Nokia

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
    Mnemonic:	config.cpp
    Abstract:	Support for reading config json.

				The config structure allows simplified parsing of the json and
				easy access to the things we belive all xAPPs will use (port
				digging form the named "interface" and control settings). This
				also supports the watching on the file and driving the user
				callback when the config file appears to have changed (write
				close on the file).

				Accessing information from the json is serialised (mutex) as
				with the random nature of file updates, we must ensure that
				we don't change the json as we're trying to read from it. Locking
				should be transparent to the user xAPP.

    Date:       27 July 2020
    Author:     E. Scott Daniels
*/

#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <memory>

#include "jhash.hpp"
#include "config.hpp"
#include "config_cb.hpp"

namespace xapp {


// ----- private things --------
/*
	Notification listener. This function is started in a thread and listens for
	changes to the config file. When it sees an interesting change (write close)
	to the file, then it will read the new set of json, parse it, and drive the
	user callback.

	We must watch the directory containing the file as if the file is edited and
	replaced it's likely saved with a different referencing inode. We'd see the
	first change, but not any subsequent changes as the inotify is based on inodes
	and not directory entries.
*/
void xapp::Config::Listener( ) {
	const struct inotify_event*	ie;	// event that popped
	int ifd;						// the inotify file des
	int	wfd;						// the watched file des
	ssize_t	n;
	char	rbuf[4096];				// large read buffer as the event is var len
	char*	dname;					// directory name
	char*	bname;					// basename
	char*	tok;

	ifd = inotify_init1( 0 );		// initialise watcher setting blocking read (no option)
	if( ifd < 0 ) {
		fprintf( stderr, "<XFCPP> ### ERR ### unable to initialise file watch %s\n", strerror( errno ) );
		return;
	}

	dname = strdup( fname.c_str() );					// defrock the file name into dir and basename
	if( (tok = strrchr( dname, '/' )) != NULL ) {
		*tok = 0;
		bname = strdup( tok+1 );
	} else {
		free (dname);
		dname = strdup( "." );
		bname = strdup( fname.c_str() );
	}

	wfd = inotify_add_watch( ifd, dname, IN_MOVED_TO | IN_CLOSE_WRITE );		// we only care about close write changes
	free (dname);

	if( wfd < 0 ) {
		fprintf( stderr, "<XFCPP> ### ERR ### unable to add watch on config file %s: %s\n", fname.c_str(), strerror( errno ) );
		free( bname );
		return;
	}

	while( true ) {
		n = read( ifd, rbuf, sizeof( rbuf ) );				// read the event
		if( n < 0  ) {
			if( errno == EAGAIN ) {
				continue;
			} else {
				fprintf( stderr, "<XFCPP> ### CRIT ### config listener read err: %s\n", strerror( errno ) );
				return;
			}
		}

		ie = (inotify_event *) rbuf;
		if( ie->len > 0 && strcmp( bname, ie->name ) == 0  ) {
			// future: lock
			auto njh = jparse( fname );							// reparse the file
			// future: unlock

			if( njh != NULL && cb != NULL ) {				// good parse, save and drive user callback
				jh = njh;
				cb->Drive_cb( *this, user_cb_data );
			}
		}
	}
}


/*
	Read a file containing json and parse into a framework Jhash.

	Using C i/o will speed this up, but I can't imagine that we need
	speed reading the config file once in a while.
	The file read comes from a stack overflow example:
		stackoverflow.com/questions/2912520/read-file-contents-into-a-string-in-c
*/
std::shared_ptr<xapp::Jhash> xapp::Config::jparse( std::string ufname ) {
	fname = ufname;

	std::ifstream ifs( fname );
	if( ! ifs.is_open() ) {
		fprintf( stderr, "<XFCPP> ### WARN ### unable to open %s; %s\n", fname.c_str(), strerror( errno ) );
	}

	std::string st( (std::istreambuf_iterator<char>( ifs ) ), (std::istreambuf_iterator<char>() ) );

	auto new_jh = std::shared_ptr<xapp::Jhash>( new xapp::Jhash( st.c_str() ) );
	return  new_jh->Parse_errors() ? NULL : new_jh;
}

/*
	Read the configuration file from what we find as the filename in the environment (assumed
	to be referenced by $XAPP_DESCRIPTOR_PATH/config-file.json. If the env var is not
	defined we assume ./.  The data is then parsed with the assumption that it's json.

	The actual meaning of the environment variable is confusing. The name is "path" which
	should mean that this is the directory in which the config file lives, but the examples
	elsewhere suggest that this is a filename (either fully qualified or relative). To prevent
	errors, we use some intelligence to determine if it's a directory name or not if it comes to it.
*/
std::shared_ptr<xapp::Jhash> xapp::Config::jparse( ) {
	const char*	data;
	std::string filename;
	struct stat sb;

	data = getenv( (const char *) "XAPP_DESCRIPTOR_PATH" );
	if( data != NULL ) {
		filename = data;
		if( stat( data, &sb ) == 0 ) {
			if( S_ISDIR( sb.st_mode ) ) {
				filename.append( "/config-file.json" );
			}
		} else {
			fprintf( stderr, "<XFCPP> ### ERR ### unable to stat env XAPP_DESCRIPTOR_PATH: %s\n", strerror( errno ) );
		}

	} else {
		filename = "./config-file.json";
	}

	return jparse( filename );
}

// --------------------- construction, destruction -------------------------------------------

/*
	By default expect to find XAPP_DESCRIPTOR_PATH in the environment and assume that it
	is the directory name where we can find config-file.json. The build process will
	read and parse the json allowing the user xAPP to invoke the supplied  "obvious"
	functions to retrieve data.  If there is something in an xAPP's config that isn't
	standard, it can get the raw Jhash object and go at it directly. The idea is that
	the common things should be fairly painless to extract from the json goop.
*/
xapp::Config::Config() :
	jh( jparse() )
{ /* empty body */ }

/*
	Similar, except that it allows the xAPP to supply the filename (testing?)
*/
xapp::Config::Config( const std::string& fname) :
	jh( jparse( fname ) )
{ /* empty body */ }


/*
	Read and return the raw file blob as a single string. User can parse, or do
	whatever they need (allows non-json things if necessary).
*/
std::string xapp::Config::Get_contents( ) const {
	std::string rv = "";

	if( ! fname.empty() ) {
		std::ifstream ifs( fname );
		std::string st( (std::istreambuf_iterator<char>( ifs ) ), (std::istreambuf_iterator<char>() ) );
		rv = st;
	}

	return rv;
}

// ----- convience function for things we think an xAPP will likely need to pull from the config


/*
	Suss out the port for the named "interface". The interface is likely the application
	name.
*/
std::string xapp::Config::Get_port( const std::string& name ) const {
	int i;
	int	nele = 0;
	double value;
	std::string rv = "";		// result value
	std::string pname;			// element port name in the json

	if( jh == NULL ) {
		return rv;
	}

	jh->Unset_blob();
	if( jh->Set_blob( (const char *) "messaging" ) ) {
		nele = jh->Array_len( (const char *) "ports" );
		for( i = 0; i < nele; i++ ) {
			if( jh->Set_blob_ele( (const char *) "ports", i ) ) {
				pname = jh->String( (const char *) "name" );
				if( pname.compare( name ) == 0 ) {				// this element matches the name passed in
					value = jh->Value( (const char *) "port" );
					rv = std::to_string( (int) value );
					jh->Unset_blob( );							// leave hash in a known state
					return rv;
				}
			}

			jh->Unset_blob( );								// Jhash requires bump to root, and array reselct to move to next ele
			jh->Set_blob( (const char *) "messaging" );
		}
	}

	jh->Unset_blob();
	return rv;
}

/*
	Suss out the named string from the controls object. If the resulting value is
	missing or "", then the default is returned.
*/
std::string xapp::Config::Get_control_str( const std::string& name, const std::string& defval ) const {
	std::string value;
	std::string rv;				// result value

	rv = defval;
	if( jh == NULL ) {
		return rv;
	}

	jh->Unset_blob();
	if(    jh->Set_blob( (const char *) "controls" ) && jh->Exists( name.c_str() ) )  {
		value = jh->String( name.c_str() );
		if( value.compare( "" ) != 0 ) {
			rv = value;
		}
	}

	jh->Unset_blob();
	return rv;
}

/*
	Convenience funciton without default. "" returned if not found.
	No default value; returns "" if not set.
*/
std::string xapp::Config::Get_control_str( const std::string& name ) const {
	return Get_control_str( name, "" );
}

/*
	Suss out the named field from the controls object with the assumption that it is a boolean.
	If the resulting value is missing then the defval is used.
*/
bool xapp::Config::Get_control_bool( const std::string& name, bool defval ) const {
	bool rv;				// result value

	rv = defval;
	if( jh == NULL ) {
		return rv;
	}

	jh->Unset_blob();
	if( jh->Set_blob( (const char *) "controls" )  &&  jh->Exists( name.c_str() ) )  {
		rv = jh->Bool( name.c_str() );
	}

	jh->Unset_blob();
	return rv;
}


/*
	Convenience function without default.
*/
bool xapp::Config::Get_control_bool( const std::string& name ) const {
	return Get_control_bool( name, false );
}


/*
	Suss out the named field from the controls object with the assumption that it is a value (float/int).
	If the resulting value is missing then the defval is used.
*/
double xapp::Config::Get_control_value( const std::string& name, double defval ) const {

	auto rv = defval;				// return value; set to default
	if( jh == NULL ) {
		return rv;
	}

	jh->Unset_blob();
	if( jh->Set_blob( (const char *) "controls" )  && jh->Exists( name.c_str() ) )  {
		rv = jh->Value( name.c_str() );
	}

	jh->Unset_blob();
	return rv;
}


/*
	Convenience function. If value is undefined, then 0 is returned.
*/
double xapp::Config::Get_control_value( const std::string& name ) const {
	return Get_control_value( name, 0.0 );
}


// ---- notification support ---------------------------------------------------------------


/*
	Accept the user's notification function, and data that it needs (pointer to
	something unknown), and stash that as a callback.

	The fact that the user xAPP registers a callback also triggers the creation
	of a thread to listen for changes on the config file.
*/
void xapp::Config::Set_callback( notify_callback usr_func, void* usr_data ) {
	cb = std::unique_ptr<Config_cb>( new Config_cb( usr_func, usr_data ) );
	user_cb_data = usr_data;

	if( listener == NULL ) {				// start thread if needed
		listener = new std::thread( &xapp::Config::Listener, this );
	}
}

} // namespace
