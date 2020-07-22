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
	Mnemonic:	metrics.hpp
	Abstract:	Headers for the metrics class.
				This class provides a simple interface for an xAPP to compose and
				send metrics. The class allows the xAPP to name the metric and to
				provide one or more key/value pairs which are sent to the metrics
				gateway (munchkin) when Send() is invoked. The class hides the
				true message structure that the munchkin desires from the xAPP.

				The munchkin endpoint is assumed to be a routable target and thus
				normal RMR messages (with appropriate type) are sent.

	Date:		20 July 2020
	Author:		E. Scott Daniels
*/

#ifndef _XAPP_METRICS_HPP
#define _XAPP_METRICS_HPP


#include <iostream>
#include <string>

#include "msg_component.hpp"

namespace xapp {

// ------------------------------------------------------------------------

/*
This class is loosely tied to the json expected by the munchkin. See the munchkin
documentation for explicit information, but this will help to understand a bit:
	{
		"reporter": "<string>",
		"generator" "<string>",
		"timestamp": double,
		"data": [
			{
				"id": "<string>",
				"type": "<string>",
				"value": double
			},...
		]
	}
*/

class Metrics {
	private:
		std::shared_ptr<Message> msg;		// message to send

											// data for the payload
		std::string reporter;				// this application as it's reporting the metrics
		std::string source;					// source of the measurement if not the reporter
		std::string	data;					// key/value/type tuples "pushed"  waiting for send


		int build_payload( xapp::Msg_component payload, int payload_len );

	public:

		Metrics( std::shared_ptr<Message> msg );										// builders
		Metrics( std::shared_ptr<Message> msg, std::string msource );					// allow xapp to supply metric source
		Metrics( std::shared_ptr<Message> msg, std::string reporter, std::string msource ); // allow xapp to supply all

		Metrics( const Metrics& soi );					// copy to newly created instance
		Metrics& operator=( const Metrics& soi );		// copy operator
		Metrics( Metrics&& soi );						// mover
		Metrics& operator=( Metrics&& soi );			// move operator
		~Metrics();										// destroyer


		void Set_source( std::string new_source );
		void Set_reporter( std::string new_reporter );
		void Push_data( std::string key, double value );


		bool Send( );
};

} // namespace

#endif
