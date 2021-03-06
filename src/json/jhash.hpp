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
	Mnemonic:	jhash.hpp
	Abstract:	This class provides the ability to parse a json string into
				a hashtable, and exposes various functions that can be used
				to read the data from the hash.

	Date:		26 June 2020
	Author:		E. Scott Daniels
*/

#ifndef _JHASH_HPP
#define _JHASH_HPP


#include <string>

namespace xapp {

// ------------------------------------------------------------------------

class Jhash {
	private:
		void*	st = NULL;							// the resulting symbol table generated by parse
		void*	master_st = NULL;					// if user switches to a sub-blob; this tracks the original root st

		Jhash& operator=( const Jhash& soi );	// jhashes cannot be copied because of underlying symbol table goo
		Jhash( const Jhash& soi );

	public:

		Jhash( const char* jblob );					// builder
		Jhash( Jhash&& soi );						// mover
		Jhash& operator=( Jhash&& soi );			// move operator
		~Jhash();									// destruction

		bool Set_blob( const char* name );					// blob/root selection
		void Unset_blob( );
		bool Set_blob_ele( const char* name, int eidx );	// set from an array element

		const bool Parse_errors( );
		void Dump();

		std::string String( const char* name );				// value fetching
		double Value( const char* name );
		bool Bool( const char* name );

		bool Exists( const char* name );					// presence checking
		bool Is_missing( const char* name );

		bool Is_bool( const char* name );					// type checking functions
		bool Is_null( const char* name );
		bool Is_string( const char* name );
		bool Is_value( const char* name );

		int Array_len( const char* name );

		bool Is_bool_ele( const char* name, int eidx );		// type of array element checks
		bool Is_null_ele( const char* name, int eidx );
		bool Is_string_ele( const char* name, int eidx );
		bool Is_value_ele( const char* name, int eidx );

		bool Bool_ele( const char* name, int eidx );			 // array oriented sussing functions
		std::string String_ele( const char* name, int eidx );
		double Value_ele( const char* name, int eidx );
};



} // namespace
#endif
