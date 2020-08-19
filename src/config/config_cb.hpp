
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
	Mnemonic:	callback.hpp
	Abstract:	Manages config notification callback data and such.
				This is a bit of over kill because, unlike the message
				receipt callbacks, there is only one potential callback
				for the config. We could manage this inside of the conf
				class, but if there is ever the need to have multiple
				callbacks, the base is set.

	Date:		27 July 2020
	Author:		E. Scott Daniels
*/


#ifndef _XAPP_CONF_CB_HPP
#define _XAPP_CONF_CB_HPP

#include <memory>

namespace xapp {

class Config;

/*
	Describes the user function that we will invoke
*/
typedef void(*notify_callback)( xapp::Config& c, void* user_data );

class Config_cb {

	private:
		notify_callback user_fun;
		void*	udata;									// user data

	public:
		Config_cb( notify_callback cbfun, void* user_data );		// builder
		void Drive_cb( xapp::Config& c, void* user_data ) const;	// invokers
};

} // namespace

#endif

