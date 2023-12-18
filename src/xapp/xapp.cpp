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
using namespace std;
// --------------- builders -----------------------------------------------

/*
	If wait4table is true, then the construction of the object does not
	complete until the underlying transport has a new copy of the route
	table.

	If port is nil, then the default port is used (4560).
*/
Xapp::Xapp( const char* port, bool wait4table ): Messenger(port, false) {
	thread regThread(&Xapp::RegisterXapp, this);
	regThread.detach();

	if( wait4table ) {
		this->Wait_for_cts( 0 );
	}
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
	thread** tinfo;				// each thread we'll start

	tinfo = new thread* [nthreads-1];

	for( i = 0; i < nthreads - 1; i++ ) {				// thread for each n-1; last runs here
		tinfo[i] = new thread( &Xapp::Listen, this );
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

string Xapp::GetService(string host, const char* service) {
	string service_url = "";
	string app_namespace = (getenv("APP_NAMESPACE") == nullptr) ? "ricxapp" : string(getenv("APP_NAMESPACE"));

	if (host != "") {
		transform(host.begin(), host.end(), host.begin(), ::toupper);
		transform(app_namespace.begin(), app_namespace.end(), app_namespace.begin(), ::toupper);

		char buffer[100];
		sprintf(buffer, service, app_namespace.c_str(), host.c_str());

		string endpoint_var = string(buffer);
		replace(endpoint_var.begin(), endpoint_var.end(), '-', '_');

		char* env_value = getenv(endpoint_var.c_str());

		char* token = strtok(env_value, "//");

		if (token != nullptr) {
			token = strtok(NULL, "//");
		}

		service_url = string(token);
	}

	return service_url;
}
        

bool Xapp::Register() {
	string hostname = string(getenv("HOSTNAME"));
	json jsonConfig = json::parse(config.Get_contents());

	string pltnamespace = (getenv("PLT_NAMESPACE") == nullptr) ? "ricplt" : string(getenv("PLT_NAMESPACE"));

	string http_endpoint = this->GetService(hostname, SERVICE_HTTP);
	string rmr_endpoint = this->GetService(hostname, SERVICE_RMR);

	cout	<< "config details hostname : " << hostname
				<< ", xappname: " << jsonConfig["name"]
				<< ", xappversion : " << jsonConfig["version"]
				<< ", pltnamespace : " << pltnamespace
				<< ", http_endpoint: " << http_endpoint
				<< ", rmr_endpoint: " << rmr_endpoint
				<< endl;

	json* request_string = new json
		{
			{"appName", hostname},
			{"appVersion", jsonConfig["version"]},
			{"configPath", CONFIG_PATH},
			{"appInstanceName", jsonConfig["name"]},
			{"httpEndpoint", http_endpoint},
			{"rmrEndpoint", rmr_endpoint},
			{"config", config.Get_contents()}

		};

	cout << "REQUEST:" << request_string->dump() << endl;

	// sending request
	

	char buffer[100];
	sprintf(buffer, REGISTER_PATH, pltnamespace.c_str(), pltnamespace.c_str());
	string register_url = string(buffer);
	xapp::cpprestclient client(register_url);

	const json& const_request_string = *request_string;

	xapp::response_t resp = client.do_post( const_request_string, "" ); // we already have the full path in ts_control_ep

	return resp.status_code == 201;
	
}

/*
	Register the xapp with Appmgr. 
*/
void Xapp::RegisterXapp() {
	bool isReady = false; //placeholder for a member attribute with value set via Ready callback

	while (not isReady) {
		sleep(5);
		if (this->Register()) {
			isReady = true;
			cout<<"Registration done, proceeding with startup ..."<<endl;
			break;
		}
		
	}
}