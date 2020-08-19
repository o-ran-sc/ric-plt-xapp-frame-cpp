
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
	Mnemonic:	notify_cb.cpp
	Abstract:	Notification drivers.

	Date:		27 July 2020
	Author:		E. Scott Daniels
*/

//#include <cstdlib>

#include "config.hpp"
#include "config_cb.hpp"

namespace xapp {

/*
	Builder.
*/
Config_cb::Config_cb( notify_callback ufun, void* user_data ) :
		user_fun( ufun ),
		udata( user_data )
{ /* empty body */  }


/*
	there is nothing to be done from a destruction perspective, so no
	destruction function needed at the moment.
*/

/*
	Drive_cb will invoke the callback and pass along the stuff passed here.
	User_data may be nil which causes the data registered with the callback
	to be used.
*/
void xapp::Config_cb::Drive_cb( xapp::Config& c, void* user_data ) const {
	if( user_fun != NULL ) {
		user_fun( c, user_data == NULL ? udata : user_data );
	}
}


} // namespace
