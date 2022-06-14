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
#include <csignal>

#include <rmr/rmr.h>

#include "callback.hpp"
#include "messenger.hpp"
#include "config.hpp"
#include "utilities.hpp"
#include "restapi_client.hpp"

#define SERVICE_HTTP		"SERVICE_%s_%s_HTTP_PORT"
#define SERVICE_RMR			"SERVICE_%s_%s_RMR_PORT"
#define LEN_TCP_SUBSTR		strlen("tcp://")
#define DEFAULT_PLT_NS		"RICXPLT"
#define DEFAULT_XAPP_NS		"RICXAPP"
#define APPMGR_HTTP			"http://service-%s-appmgr-http.ricplt:8080/ric/v1"
#define PLT_NS_ENV			"PLT_NAMESPACE"
#define XAPP_NS_ENV			"XAPP_NAMESPACE"
#define CONFIG_PATH			"/ric/v1/config"

class Xapp : public xapp::Messenger {

	private:
		std::string m_appmgr_http;
		std::string m_xappName;
		static Xapp& me; //use to call _signalHandler() from signalHandler()

		Xapp( const Xapp& soi );
		Xapp& operator=( const Xapp& soi );
		std::shared_ptr<xapp::Config> sp_cfg = NULL;
		virtual void registerXapp() final;
		virtual void deregisterXapp() final;

	public:
		Xapp(const char* port, bool wait4table);
		Xapp(std::shared_ptr<xapp::Config> cfg);
		Xapp(xapp::Config& cfg);
		~Xapp();									// destroyer

		void Init();
		void Run( int nthreads );					// message listen driver
		void Halt( );								// force to stop
		void _signalHandler(int signum);
		static void signalHandler(int signum) {
			me._signalHandler(signum);
			return;
		}
};

#endif
