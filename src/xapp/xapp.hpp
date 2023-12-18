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
	Mnemonic:	xapp.hpp
	Abstract:	Headers for the xapp class. This class extends the messenger class
				as most of the function of this class would be just passing through
				calls to the messenger object.

	Date:		13 March 2020
	Author:		E. Scott Daniels
*/

#ifndef _xapp_hpp
#define _xapp_hpp


#include <iostream>
#include <string>
#include <mutex>
#include <map>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/schema.h>
#include <rapidjson/reader.h>
#include <rapidjson/prettywriter.h>
#include <rmr/rmr.h>
#include "config.hpp"
#include "callback.hpp"
#include "RestClient.h"
#include "messenger.hpp"

class Xapp : public xapp::Messenger {

	private:
		std::string name;
		xapp::Config config;

		// copy and assignment are PRIVATE because we cant "clone" the listen environment
		Xapp( const Xapp& soi );
		Xapp& operator=( const Xapp& soi );
		bool Register();
		std::string GetService(std::string host, const char* service);
	public:
		const char* SERVICE_HTTP = "SERVICE_%s_%s_HTTP_PORT";
		const char* SERVICE_RMR = "SERVICE_%s_%s_RMR_PORT";
		const std::string CONFIG_PATH = "/ric/v1/config";
		const std::string REGISTER_PATH = "http://service-%s-appmgr-http.%s:8080/ric/v1/register";

		Xapp( const char* listen_port, bool wait4rt );	// builder
		Xapp( );
		~Xapp();									// destroyer
		
		void RegisterXapp( );							// register xApp with Appmgr
		void Run( int nthreads );					// message listen driver
		void Halt( );								// force to stop
};

#endif
