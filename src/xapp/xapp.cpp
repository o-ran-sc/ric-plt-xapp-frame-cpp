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

#include "xapp.hpp"
using namespace xapp;

// --------------- private -----------------------------------------------------



// --------------- builders -----------------------------------------------
/*
	If wait4table is true, then the construction of the object does not
	complete until the underlying transport has a new copy of the route
	table.

	If port is nil, then the default port is used (4560).
*/
Xapp::Xapp( const char* port, bool wait4table ) : Messenger( port, wait4table ) {
	// nothing to do; all handled in Messenger constructor
}

Xapp::Xapp(std::shared_ptr<Config> cfg) : Messenger(cfg->Get_port("rmr-data").c_str(), false) {
	sp_cfg = cfg;
	Init();
	registerXapp();
}

Xapp::Xapp(Config& cfg) : Messenger(cfg.Get_port("rmr-data").c_str(), false) {
	sp_cfg = std::make_shared<Config>(std::move(cfg));
	Init();
	registerXapp();
}

/*
	Destroyer.
*/
Xapp::~Xapp() {
	deregisterXapp();
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

void Xapp::Init() {

	/*Init some require param*/
	me = *this;
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	std::string pltNS = getEnvironmentVariable(PLT_NS_ENV, DEFAULT_PLT_NS);
	bzero(buf, sizeof(buf));
	snprintf(buf, sizeof(buf), APPMGR_HTTP, pltNS.c_str());
	m_appmgr_http = buf;
	m_xappName = sp_cfg->Get_string("xapp_name", "");

	std::signal(SIGTERM, Xapp::signalHandler);
}

void Xapp::registerXapp() {

	char buf[1024];
	auto client = std::make_shared<RestClient>(m_appmgr_http);
	auto sp_register = std::make_shared<RestModel::RegisterData>();

	std::string xappNS= getEnvironmentVariable(XAPP_NS_ENV, DEFAULT_XAPP_NS);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), SERVICE_HTTP, xappNS.c_str(), m_xappName.c_str());
	std::string httpService = getEnvironmentVariable(toUpper(buf), "");

	if(0 == httpService.compare("") || httpService.length() < LEN_TCP_SUBSTR)
	{
		std::cerr << __func__ << " " << __LINE__ << "Cannot get " << buf << "\n";
		return;
	}

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), SERVICE_RMR, xappNS.c_str(), m_xappName.c_str());
	std::string rmrService = getEnvironmentVariable(toUpper(buf), "");

	if(0 == rmrService.compare("") || rmrService.length() < LEN_TCP_SUBSTR)
	{
		std::cerr << __func__ << " " << __LINE__ << "Cannot get " << buf << "\n";
		return;
	}

	sp_register->setAppName(m_xappName)
			.setHttpEndpoint(httpService.substr(LEN_TCP_SUBSTR, std::string::npos))
			.setRmrEndpoint(rmrService.substr(LEN_TCP_SUBSTR, std::string::npos))
			.setAppInstanceName(m_xappName)
			.setAppVersion(sp_cfg->Get_string("version", ""))
			.setConfigPath(CONFIG_PATH)
			.setConfig(sp_cfg->Get_contents());

	std::cerr << sp_register->toJson().serialize() << std::endl;
	std::string path = "/register";
	client->doPost(path, sp_register->toJson().serialize()).then([=](pplx::task<std::string> response)
	{
		try
		{
			std::cerr << "Register xApp successful" << "\n";
			std::cerr << response.get() << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << __func__ << " catch exception: " << e.what() << std::endl;
		}
	}).wait();
}

void Xapp::deregisterXapp() {

	auto client = std::make_shared<RestClient>(m_appmgr_http);
	auto sp_deregister = std::make_shared<RestModel::DeregisterData>();

	sp_deregister->setAppName(m_xappName)
			.setAppInstanceName(m_xappName);
	std::cerr << sp_deregister->toJson().serialize() << std::endl;

	std::string path = "/deregister";
	client->doPost(path, sp_deregister->toJson().serialize()).then([=](pplx::task<std::string> response)
	{
		try
		{
			std::cerr << "Deregister xApp successful" << "\n";
			std::cerr << response.get() << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << __func__ << " catch exception: " << e.what() << std::endl;
		}
	}).wait();
	return;
}

void Xapp::_signalHandler(int signum) {

	switch(signum)
	{
		case SIGTERM:
			deregisterXapp();
			exit(signum);
			break;
		default:
			exit(signum);
			break;
	}
	return;
}
