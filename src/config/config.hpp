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
    Mnemonic:	config.
    Abstract:	Header for the config reader/watcher class.

    Date:       27 July 2020
    Author:     E. Scott Daniels
*/

#ifndef XAPP_CONFIG_HPP
#define XAPP_CONFIG_HPP


#include <iostream>
#include <thread>

#include "jhash.hpp"
#include "config.hpp"
#include "config_cb.hpp"

namespace xapp {

#define MAX_PFNAME	(4096 + 256)		// max path name and max filname + nil for buffer allocation

class Config {
	std::string	fname;				// the file name that we'll listen to
	std::thread* listener;			// listener thread info

	std::shared_ptr<Jhash>	jh;		// the currently parsed json from the config
	std::unique_ptr<Config_cb> cb;	// info needed to drive user code when config change noticed
	void*	user_cb_data;			// data that the caller wants passed on notification callback

	// -----------------------------------------------------------------------
	private:
		std::shared_ptr<xapp::Jhash>  jparse( std::string fname );
		std::shared_ptr<xapp::Jhash>  jparse( );
		void Listener( );

	public:
		Config();						// builders
		Config( std::string fname);

		bool Get_control_bool( std::string name, bool defval );
		bool Get_control_bool( std::string name );

		std::string Get_contents( );

		std::string Get_control_str( std::string name, std::string defval );
		std::string Get_control_str( std::string name );

		double Get_control_value( std::string name, double defval );
		double Get_control_value( std::string name );

		std::string Get_port( std::string name );

		void Set_callback( notify_callback usr_func, void* usr_data );
};


} // namespace



#endif
