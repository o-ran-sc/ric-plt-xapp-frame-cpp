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
	Mnemonic:	xapp.cpp
	Abstract:	The main xapp class.
				This class actually extends the messenger class as most of what
				would be done here is just passing through to the messenger.

	Date:		13 March 2020
	Author:		E. Scott Daniels
*/

#include <algorithm>
#include <unistd.h>
#include <thread>

#include <rmr/RIC_message_types.h>
#include "RestClient.h"
#include "xapp.hpp"
using json = nlohmann::json;

// --------------- builders -----------------------------------------------

/*
	If wait4table is true, then the construction of the object does not
	complete until the underlying transport has a new copy of the route
	table.

	If port is nil, then the default port is used (4560).
*/
Xapp::Xapp( const char* port, bool wait4table ) {
	std::thread regThread(&Xapp::RegisterXapp, this);
	regThread.join();
	Messenger(port, wait4table);
}

/*
	Destroyer.
*/
Xapp::~Xapp() {
	// nothing to destroy; superclass destruction is magically invoked.
}

/*
	Start n threads, each running a listener which will drive callbacks.

	The final listener is started in the main thread; in otherwords this
	function won't return unless that listener crashes.
*/
void Xapp::Run( int nthreads ) {
	int i;
	std::thread** tinfo;				// each thread we'll start

	tinfo = new std::thread* [nthreads-1];

	for( i = 0; i < nthreads - 1; i++ ) {				// thread for each n-1; last runs here
		tinfo[i] = new std::thread( &Xapp::Listen, this );
	}

	this->Listen();						// will return only when halted

	for( i = 0; i < nthreads - 1; i++ ) {				// wait for others to stop
		tinfo[i]->join();
	}

	delete[] tinfo;
}

/*
	Halt the xapp. This will drive the messenger's stop function to prevent any
	active listeners from running, and will shut things down.
*/
void Xapp::Halt() {
	this->Stop();			// messenger shut off
}

std::string Xapp::GetService(std::string host, std::string service) {
	std::string service_url = "";
	std::string app_namespace = std::string(getenv("APP_NAMESPACE"));

	if (app_namespace == "") {
		app_namespace = "ricxapp";
	}

	if (host != "") {
		std::transform(host.begin(), host.end(), host.begin(), ::toupper);
		std::transform(app_namespace.begin(), app_namespace.end(), app_namespace.begin(), ::toupper);

		std::string endpoint_var;
		endpoint_var.reserve(128);
		std::sprintf((char*)&endpoint_var, SERVICE_HTTP.c_str(), app_namespace, host);

		std::replace(endpoint_var.begin(), endpoint_var.end(), '-', '_');

		char* env_value = getenv((char*)&endpoint_var);

		char* token = strtok((char*)&env_value, "//");

		if (token != NULL) {
			token = strtok(NULL, "//");
		}

		service_url = std::string(token);
	}

	return service_url;
}
        

bool Xapp::Register() {
	std::string hostname = std::string(getenv("HOSTNAME"));
	std::string xappname = config.Get_control_str("name");
	std::string xappversion = config.Get_control_str("version");

	std::string pltnamespace = std::string(getenv("PLT_NAMESPACE"));

	if (pltnamespace == "") {
		pltnamespace = "ricplt";
	}

	std::string http_endpoint = this->GetService(hostname, SERVICE_HTTP);
	std::string rmr_endpoint = this->GetService(hostname, SERVICE_RMR);

	std::cout	<< "config details hostname : " << hostname 
				<< ", xappname: " << xappname 
				<< ", xappversion : " << xappversion 
				<< "pltnamespace : " << pltnamespace 
				<< ", http_endpoint: " << http_endpoint 
				<< ", rmr_endpoint: " << rmr_endpoint 
				<< std::endl;

	json* request_string = new json
		{
			{"appName", hostname},
			{"appVersion", xappversion},
			{"configPath", CONFIG_PATH},
			{"appInstanceName", xappname},
			{"httpEndpoint", http_endpoint},
			{"rmrEndpoint", rmr_endpoint},
			{"config", config.Get_contents()}

		};

	std::cout << "REQUEST STRING :" << request_string->dump() << std::endl;

	// sending request
	std::string register_url;

	register_url.reserve(128);
	std::sprintf(&register_url[0], REGISTER_PATH.c_str(), pltnamespace, pltnamespace);

	xapp::cpprestclient client(register_url);

	const json& const_request_string = *request_string;

	xapp::response_t resp = client.do_post( const_request_string, "" ); // we already have the full path in ts_control_ep

	if( resp.status_code == 200 ) {
		// ============== DO SOMETHING USEFUL HERE ===============
		// Currently, we only print out the HandOff reply
		rapidjson::Document document;
		document.Parse( resp.body.dump().c_str() );
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
		document.Accept( writer );
		std::cout << "[INFO] HandOff reply is " << s.GetString() << std::endl;
		return 1;
	} else {
		std::cout << "[ERROR] Unexpected HTTP code " << resp.status_code << " from " << \
				client.getBaseUrl() << \
				"\n[ERROR] HTTP payload is " << resp.body.dump().c_str() << std::endl;
	}

	return 0;
	
}

/*
	Register the xapp with Appmgr. 
*/
void Xapp::RegisterXapp() {
	bool isReady = false;

	while (not isReady) {
		sleep(5);
		if (this->Register()) {
			std::cout<<"Registration done, proceeding with startup ..."<<std::endl;
			break;
		}
		
	}
}