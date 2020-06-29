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
	Mnemonic:	jwrapper.h
	Abstract:	Header file for jwrapper; mostly prototypes.

	Date:		26 June 2020
	Author:		E. Scott Daniels
*/


#ifdef __cplusplus
extern "C" {
#endif

#ifndef _JWRAPPER_H
#define _JWRAPPER_H


//---------------- jwrapper -------------------------------------------------------------------------------
extern void jw_nuke( void* st );
extern void* jw_new( const char* json );

extern int jw_bool_ele( void* st, const char* name, int idx );
extern void jw_dump( void* st );
extern int jw_missing( void* st, const char* name );
extern int jw_exists( void* st, const char* name );
extern char* jw_string( void* st, const char* name );
extern double jw_value( void* st, const char* name );
extern void* jw_blob( void* st, const char* name );
extern char* jw_string_ele( void* st, const char* name, int idx );
extern double jw_value_ele( void* st, const char* name, int idx );
extern void* jw_obj_ele( void* st, const char* name, int idx );
extern int jw_array_len( void* st, const char* name );

extern int jw_is_string( void* st, const char* name );
extern int jw_is_value( void* st, const char* name );
extern int jw_is_bool( void* st, const char* name );
extern int jw_is_null( void* st, const char* name );

extern int jw_is_string_ele( void* st, const char* name, int idx );
extern int jw_is_value_ele( void* st, const char* name, int idx );
extern int jw_is_bool_ele( void* st, const char* name, int idx );
extern int jw_is_null_ele( void* st, const char* name, int idx );

// ---------------- jw_xapi ---------------------------------------------------------------------------------
extern int jwx_get_bool( void* jblob, char const* field_name, int def_value );
extern double jwx_get_value( void* jblob, char const* field_name, double def_value );
extern int jwx_get_ivalue( void* jblob, char const* field_name, int def_value );
extern char* jwx_get_value_as_str( void* jblob, char const* field_name, char const* def_value, int  fmt );
extern char* jwx_get_str( void* jblob, char const* field_name, char const* def_value );

#endif

#ifdef __cplusplus
}
#endif

