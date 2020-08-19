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
	Mnemonic:	jhash.cpp
	Abstract:	This class provides the ability to parse a json string into
				a hashtable, and exposes various functions that can be used
				to read the data from the hash.

	Date:		26 June 2020
	Author:		E. Scott Daniels
*/


#include <string>

#include "jwrapper.h"
#include "jhash.hpp"


namespace xapp {


// ------------------------------------------------------------------------



// ----------- construction/destruction housekeeping things -------------------
/*
	Accept a string that contains valid json. Causes it to be parsed
	after which the functions provided by the class can be used to
	suss out the values.
*/
xapp::Jhash::Jhash( const char* jbuf ) :
	st( jw_new( jbuf ) )
{ /* empty body */ }


/*
	Move constructor.
*/
Jhash::Jhash( Jhash&& soi ) :
	master_st( soi.master_st ),
	st( soi.st )
{
	soi.st = NULL;						// prevent closing of RMR stuff on soi destroy
	soi.master_st = NULL;
}

/*
	Move Operator.
*/
Jhash& Jhash::operator=( Jhash&& soi ) {
	if( this != &soi ) {						// cannot do self assignment
		master_st = soi.master_st;
		st = soi.st;

		soi.st = NULL;						// prevent closing of RMR stuff on soi destroy
		soi.master_st = NULL;
	}

	return *this;
}

/*
	Blow it away.
*/
xapp::Jhash::~Jhash() {
	if( master_st != NULL ) {		// revert blob set if needed
		st = master_st;
	}

	jw_nuke( st );
	st = NULL;
	master_st = NULL;
}


// ------------ public API ---------------------------------------------------

// IMPORTANT: all underlying jwrapper functions check for nil st and name pointers
//				so that is NOT needed in this code.


// --- root control ----------------------------------------------------------
/*
	Sets the "root" to the named blob. Until unset, all subsequent
	calls to Jhash functions (e.g. Is_missing()) will use this for the
	look up. Returns true if name exists and was indeed a blob.

	It is legit to call set multiple times without unsetting; set
	overlays with the named root; unset needs only to be called to
	return to the top level.
*/
bool xapp::Jhash::Set_blob( const char* name ) {
	void*	bst;						// blob symbol table

	if( master_st == NULL ) {			// must capture master
		master_st = st;
	}

	if( (bst = jw_blob( st, name )) != NULL ) {
		st = bst;
		return true;
	}

	return false;
}

/*
	Return the suss root (blob root) to the root of the symtab.
*/
void xapp::Jhash::Unset_blob( ) {
	if( master_st != NULL ) {
		st = master_st;
	}
}

// ---------------- debugging and sanity checks ---------------------------------------
/*
	Returns true if there were parse errors which resulted in an empty
	or non-existant hash.

	Right now we don't have much to work with other than checking for a
	nil table.
*/
const bool xapp::Jhash::Parse_errors( ) {
	return st == NULL;
}

/*
	Dump the selected blob as much as we can.
*/
void xapp::Jhash::Dump() {
	jw_dump( st );
}

// ---------------- type testing -----------------------------------------

/*
	These funcitons return true if the named object in the current blob
	is the indicated type
*/
bool xapp::Jhash::Is_value( const char* name ) {
	return jw_is_value( st, name ) == 1;
}

bool xapp::Jhash::Is_bool( const char* name ) {
	return jw_is_bool( st, name ) == 1;
}

bool xapp::Jhash::Is_null( const char* name ) {
	return jw_is_null( st, name ) == 1;
}

bool xapp::Jhash::Is_string( const char* name ) {
	return jw_is_string( st, name ) == 1;
}

/*
	These functions return true if the indicated element in the array
	<name> is the indicated type.
*/
bool xapp::Jhash::Is_string_ele( const char* name, int eidx ) {
	return jw_is_string_ele( st, name, eidx ) == 1;
}

bool xapp::Jhash::Is_value_ele( const char* name, int eidx ) {
	return jw_is_value_ele( st, name, eidx ) == 1;
}

bool xapp::Jhash::Is_bool_ele( const char* name, int eidx ) {
	return jw_is_bool_ele( st, name, eidx ) == 1;
}

bool xapp::Jhash::Is_null_ele( const char* name, int eidx ) {
	return jw_is_null_ele( st, name, eidx ) == 1;
}


// ---------------- presence ------------------------------------------------
/*
	Returns true if the named element is in the hash.
*/
bool xapp::Jhash::Exists( const char* name ) {
	return jw_exists( st, name ) == 1;
}

/*
	Returns true if the named element is not in the hash.
*/
bool xapp::Jhash::Is_missing( const char* name ) {
	return jw_missing( st, name ) == 1;
}

// ---------------- value sussing ----------------------------------------

/*
	Returns the boolean value for the object if it is a bool; false otherwise.

	Symtab saves bool values as 1 for true and doesn't provide a bool fetch
	function. So, fetch the value and return true if it is 1.
*/
bool xapp::Jhash::Bool( const char* name ) {
	int v;
	v = (int) jw_value( st, name );

	return v == 1;
}

/*
	Returns a C++ string to the named object; If the element is not
	in the hash an empty string is returned.
*/
std::string xapp::Jhash::String( const char* name ) {
	std::string rv = "";
	const char*	hashv;

	if( (hashv = jw_string( st, name )) != NULL ) {
		rv = std::string( hashv );
	}

	return rv;
}


/*
	Returns the value assocated with the named object; If the element is not
	in the hash 0 is returned.
*/
double xapp::Jhash::Value( const char* name ) {
	return jw_value( st, name );
}

// ------ array related things --------------------------------------------

/*
	Return the length of the named array, or -1 if it doesn't exist.
*/
int xapp::Jhash::Array_len( const char* name ) {
	return jw_array_len( st, name );
}


/*
	Sets the blob in the array <name>[eidx] to the current reference blob.
*/
bool xapp::Jhash::Set_blob_ele( const char* name, int eidx ) {
	void*	bst;

	if( (bst = jw_obj_ele( st, name, eidx )) != NULL ) {
		if( master_st == NULL ) {			// must capture master
			master_st = st;
		}

		st = bst;
		return true;
	}

	return false;
}
/*
	Return the string at index eidx in the array <name>.
*/
std::string xapp::Jhash::String_ele( const char* name, int eidx ) {
	std::string rv = "";
	const char*	hashv;

	if( (hashv = jw_string_ele( st, name, eidx )) != NULL ) {
		rv = std::string( hashv );
	}

	return rv;
}

/*
	Return the value at index eidx in the array <name>.
*/
double xapp::Jhash::Value_ele( const char* name, int eidx ) {
	return jw_value_ele( st, name, eidx );
}

/*
	Return the bool value at index eidx in the array <name>.
*/
bool xapp::Jhash::Bool_ele( const char* name, int eidx ) {
	return jw_bool_ele( st, name, eidx ) == 1;
}



} // namespace
