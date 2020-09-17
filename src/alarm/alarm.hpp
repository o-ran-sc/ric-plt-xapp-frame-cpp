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
	Mnemonic:	alarm.hpp
	Abstract:	Headers for the alarm class.
				This class provides for an alarm API.

	Date:		15 July 2020
	Author:		E. Scott Daniels
*/

#ifndef _XAPP_ALARM_HPP
#define _XAPP_ALARM_HPP


#include <iostream>
#include <string>

#include "msg_component.hpp"

namespace xapp {

// ------------------------------------------------------------------------

class Alarm {
	private:
		std::shared_ptr<Message> msg;		// message to send
		std::shared_ptr<char> psp;			// shared pointer to the payload to give out
		std::string endpoint =  "";			// the ip:port addr:port of the alarm collector
		int			whid = -1;

											// data for the payload
		std::string	me_id = "";				// managed element ID
		std::string app_id = "";			// application ID
		int			problem_id  = -1;		// problem ID (specific problem)
		std::string	severity = "";			// set_sev() xlates from SEV_* consts to collector's string values
		std::string info = "";				// info string supplied by user
		std::string add_info = "";			// additional information supplied by user

		int build_alarm( int action_id, xapp::Msg_component payload, int payload_len );

	public:
		static const int	SEV_CRIT = 1;			// allow translation to string on send/gen
		static const int	SEV_MAJOR = 2;
		static const int	SEV_MINOR = 3;
		static const int	SEV_WARN = 4;
		static const int	SEV_CLEAR = 5;
		static const int	SEV_DEFAULT = 6;

		static const int	ACT_RAISE = 1;			// action const map to alarm manager strings
		static const int	ACT_CLEAR = 2;
		static const int	ACT_CLEAR_ALL = 3;

		explicit Alarm( std::shared_ptr<Message> msg );		// builders
		Alarm( std::shared_ptr<Message> msg, const std::string& meid  );
		Alarm( std::shared_ptr<Message> msg, int prob_id, const std::string& meid  );

		Alarm( const Alarm& soi );					// copy to newly created instance
		Alarm& operator=( const Alarm& soi );		// copy operator
		Alarm( Alarm&& soi );						// mover
		Alarm& operator=( Alarm&& soi ) noexcept;			// move operator
		~Alarm();									// destroyer


		std::string Get_endpoint( ) const;

		void Set_additional( const std::string& new_info );
		void Set_appid( const std::string& new_id );
		void Set_info( const std::string& new_info );
		void Set_meid( const std::string& new_meid );
		void Set_problem( int new_id );
		void Set_severity( int new_sev );
		void Set_whid( int whid );


		bool Raise( );
		bool Raise( int severity, int problem, const std::string& info );
		bool Raise( int severity, int problem, const std::string& info, const std::string& additional_info );
		bool Raise_again( );

		bool Clear( );
		bool Clear( int severity, int problem, const std::string& info );
		bool Clear( int severity, int problem, const std::string& info, const std::string& additional_info );
		bool Clear_all( );


		void Dump() const;
};

} // namespace

#endif
