
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
	Mnemonic:	jwrapper.c
	Abstract:	A wrapper interface to the jsmn library which makes it a bit easier
				to use.  Parses a json string capturing the contents in a symtab.

				This code is based on the AT&T VFd open source library available
				on github.com/att/vfd.  The changes are mostly to port to the
				RMR version of symtab from VFd's version.

	Author:		E. Scott Daniels
	Date:		26 June 2020

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef DEBUG
	#define DEBUG 0
#endif

#define JSMN_STATIC 1		// jsmn no longer builds into a library; this pulls as static functions
#include <jsmn.h>
//#include <../../ext/jsmn/jsmn.h>

#include <rmr/rmr_symtab.h>

#define JSON_SYM_NAME	"_jw_json_string"
#define MAX_THINGS		1024 * 4	// max objects/elements

#define PT_UNKNOWN		0			// primative types; unknown for non prim
#define PT_VALUE		1
#define PT_BOOL			2
#define PT_NULL			3
#define PT_STRING		4

#define OBJ_SPACE		1			// space in the symbol table where json bits are stashed

extern void jw_nuke( void* st );

// ---------------------------------------------------------------------------------------

/*
	This is what we will manage in the symtab. Right now we store all values (primatives)
	as float, but we could be smarter about it and look for a decimal. Unsigned and
	differences between long, long long etc are tough.
*/
typedef struct jthing {
	int jsmn_type;				// propigated type from jsmn (jsmn constants)
	int prim_type;				// finer grained primative type (bool, null, value)
	int	nele;					// number of elements if applies
	union {
		float fv;
		void *pv;
	} v;
} jthing_t;


/*
	Given the json token, 'extract' the element by marking the end with a
	nil character, and returning a pointer to the start.  We do this so that
	we don't create a bunch of small buffers that must be found and freed; we
	can just release the json string and we'll be done (read won't leak).
*/
static char* extract( char* buf, jsmntok_t *jtoken ) {
	buf[jtoken->end] = 0;
	return &buf[jtoken->start];
}

/*
	create a new jthing and add a reference to it in the symbol table st.
	sets the number of elements to 1 by default.
*/
static jthing_t *mk_thing( void *st, char *name, int jsmn_type ) {
	jthing_t	*jtp = NULL;

	if( st != NULL &&
		name != NULL &&
		(jtp = (jthing_t *) malloc( sizeof( *jtp ) )) != NULL ) {

		if( DEBUG ) {
			fprintf( stderr, "<DBUG> jwrapper adding: %s type=%d\n", name, jsmn_type );
		}

		jtp->jsmn_type = jsmn_type;
		jtp->prim_type = PT_UNKNOWN;			// caller must set this
		jtp->nele = 1;
		jtp->v.fv = 0;

		rmr_sym_put( st, name, OBJ_SPACE, jtp );
	} else {
		fprintf( stderr, "[WARN] jwrapper: unable to create '%s' type=%d\n", name, jsmn_type );
	}

	return jtp;
}


/*
	Find the named array. Returns a pointer to the jthing that represents
	the array (type, size and pointer to actual array of jthings).
	Returns nil pointer if the named thing isn't there or isn't an array.
*/
static jthing_t* suss_array( void* st, const char* name ) {
	jthing_t* jtp = NULL;							// thing that is referenced by the symtab

	if( st != NULL &&
		name != NULL &&
		(jtp = (jthing_t *) rmr_sym_get( st, name, OBJ_SPACE )) != NULL ) {

		jtp =  jtp->jsmn_type == JSMN_ARRAY  ? jtp : NULL;
	}

	return jtp;
}

/*
	Suss an array from the hash and return the ith element.
*/
static jthing_t* suss_element( void* st, const char* name, int idx ) {
	jthing_t* jtp;									// thing that is referenced by the symtab
	jthing_t* jarray;
	jthing_t* rv = NULL;

	if( (jtp = suss_array( st, name )) != NULL &&		// have pointer
		idx >= 0 &&										// and in range
		idx < jtp->nele ) {

		if( (jarray = jtp->v.pv)  != NULL ) {
			rv = &jarray[idx];
		}
	}

	return rv;
}


/*
	Invoked for each thing in the symtab; we free the things that actually point to
	allocated data (e.g. arrays) and recurse to handle objects.
*/
static void nix_things( void* st, void* se, const char* name,  void* ele, void *data ) {
	jthing_t*	j;
	jthing_t*	jarray;
	int i;

	j = (jthing_t *) ele;
	if( j ) {
		switch( j->jsmn_type ) {
			case JSMN_ARRAY:
				if( (jarray = (jthing_t *) j->v.pv)  != NULL ) {
					for( i = 0; i < j->nele; i++ ) {					// must look for embedded objects
						if( jarray[i].jsmn_type == JSMN_OBJECT ) {
							jw_nuke( jarray[i].v.pv );
							jarray[i].jsmn_type = JSMN_UNDEFINED;			// prevent accidents
						}
					}

					free( j->v.pv );			// must free the array (arrays aren't nested, so all things in the array don't reference allocated mem)
				}
				break;

			case JSMN_OBJECT:
				jw_nuke( j->v.pv );
				j->jsmn_type = JSMN_UNDEFINED;			// prevent a double free
				break;
		}
	}
}

/*
	Invoked for each thing and prints what we can to stderr.
*/
static void dump_things( void* st, void* se, const char* name,  void* ele, void *data ) {
	jthing_t*	j;
	jthing_t*	jarray;
	int i;

	j = (jthing_t *) ele;
	if( j ) {
		fprintf( stderr, "<DBUG> jwrapper: element '%s' has ptype %d, jsmn type %d\n", name, j->prim_type, j->jsmn_type );
	} else {
		fprintf( stderr, "<DBUG> jwrapper: element has no data: '%s'\n", name );
	}
}

/*
	Real work for parsing an object ({...}) from the json.   Called by jw_new() and
	recurses to deal with sub-objects.
*/
void* parse_jobject( void* st, char *json, char* prefix ) {
	jthing_t	*jtp;			// json thing that we just created
	int		i;
	int		n;
	char	*name;				// name in the json
	char	*data;				// data string from the json
	jthing_t*	jarray;			// array of jthings we'll coonstruct
	int		size;
	int		osize;
	int		njtokens;			// tokens actually sussed out
	jsmn_parser jp;				// 'parser' object
	jsmntok_t *jtokens;			// pointer to tokens returned by the parser
	char	pname[1024];		// name with prefix
	char	wbuf[256];			// temp buf to build a working name in
	char*	dstr;				// dup'd string

	jsmn_init( &jp );			// does this have a failure mode?

	jtokens = (jsmntok_t *) malloc( sizeof( *jtokens ) * MAX_THINGS );
	if( jtokens == NULL ) {
		fprintf( stderr, "[CRI] jwrapper: cannot allocate tokens array\n" );
		return NULL;
	}

	njtokens = jsmn_parse( &jp, json, strlen( json ), jtokens, MAX_THINGS );

	if( jtokens[0].type != JSMN_OBJECT ) {				// if it's not an object then we can't parse it.
		fprintf( stderr, "[WARN] jwrapper: badly formed json; initial opening bracket ({) not detected\n" );
		return NULL;
	}

	if( DEBUG ) {
		for( i = 1; i < njtokens-1; i++ ) {
			fprintf( stderr, "<DBUG> %4d: size=%d start=%d end=%d %s\n", i, jtokens[i].size, jtokens[i].start, jtokens[i].end, extract( json, &jtokens[i] ) );
		}
	}

	for( i = 1; i < njtokens-1; i++ ) {					// we'll silently skip the last token if it's "name" without a value
		if( jtokens[i].type != JSMN_STRING ) {
			fprintf( stderr, "[WARN] jwrapper: badly formed json [%d]; expected name (string) found type=%d %s\n", i, jtokens[i].type, extract( json, &jtokens[i] ) );
			rmr_sym_free( st );
			return NULL;
		}
		name = extract( json, &jtokens[i] );
		if( *prefix != 0 ) {
			snprintf( pname, sizeof( pname ), "%s.%s", prefix, name );
			name = pname;
		}

		size = jtokens[i].size;

		i++;										// at the data token now
		switch( jtokens[i].type ) {
			case JSMN_OBJECT:				// save object in two ways: as an object 'blob' and in the current symtab using name as a base (original)
				dstr = strdup( extract( json, &jtokens[i] ) );
				snprintf( wbuf, sizeof( wbuf ), "%s_json", name );	// must stash the json string in the symtab for clean up during nuke
				rmr_sym_put( st, wbuf, OBJ_SPACE, dstr );

				parse_jobject( st, dstr, name );					// recurse to add the object as objectname.xxxx elements

				if( (jtp = mk_thing( st, name, jtokens[i].type )) != NULL &&		// create thing and reference it in current symtab
					(jtp->v.pv = (void *) rmr_sym_alloc( 255 ) ) != NULL ) {		// object is just a blob

					dstr = strdup( extract( json, &jtokens[i] ) );
					rmr_sym_put( jtp->v.pv, JSON_SYM_NAME, OBJ_SPACE, dstr );		// must stash json so it is freed during nuke()
					parse_jobject( jtp->v.pv,  dstr, "" );							// recurse acorss the string and build a new symtab

					size = jtokens[i].end;											// done with them, we need to skip them
					i++;
					while( i < njtokens-1  &&  jtokens[i].end < size ) {
						if( DEBUG ){
							fprintf( stderr, "\tskip: [%d] object element start=%d end=%d (%s)\n",
								i, jtokens[i].start, jtokens[i].end, extract( json, &jtokens[i])  );
						}
						i++;
					}

					i--;						// must allow loop to bump past the last
				}
				break;

			case JSMN_ARRAY:
				size = jtokens[i].size;		// size is burried here, and not with the name
				jtp = mk_thing( st, name, jtokens[i].type );

				i++;			// skip first ele; it is the whole array string which I don't grock the need for, but it's their code...
				if( jtp == NULL ) {
					fprintf( stderr, "[WARN] jwrapper: memory alloc error processing element [%d] in json\n", i );
					rmr_sym_free( st );
					return NULL;
				}
				jarray = jtp->v.pv = (jsmntok_t *) malloc( sizeof( *jarray ) * size );		// allocate the array
				memset( jarray, 0, sizeof( *jarray ) * size );
				jtp->nele = size;

				for( n = 0; n < size; n++ ) {								// for each array element
					jarray[n].prim_type	 = PT_UNKNOWN;						// assume not primative type
					switch( jtokens[i+n].type ) {

						case JSMN_OBJECT:
							jarray[n].v.pv = (void *) rmr_sym_alloc( 255 );
							if( jarray[n].v.pv != NULL ) {
								jarray[n].jsmn_type = JSMN_OBJECT;
								parse_jobject( jarray[n].v.pv,  extract( json, &jtokens[i+n]  ), "" );		// recurse acorss the string and build a new symtab
								osize = jtokens[i+n].end;									// done with them, we need to skip them
								i++;
								while( i+n < njtokens-1  &&  jtokens[n+i].end < osize ) {
									i++;
								}
								i--;					// allow incr at loop end
							}
							break;

						case JSMN_ARRAY:
							fprintf( stderr, "[WARN] jwrapper: [%d] array element %d is not valid type (array) is not string or primative\n", i, n );
							n += jtokens[i+n].size;			// this should skip the nested array
							break;

						case JSMN_STRING:
							data = extract( json, &jtokens[i+n] );
							jarray[n].v.pv = (void *) data;
							jarray[n].prim_type = PT_STRING;
							jarray[n].jsmn_type = JSMN_STRING;
							break;

						case JSMN_PRIMITIVE:
							data = extract( json, &jtokens[i+n] );
							switch( *data ) {
								case 'T':
								case 't':
									jarray[n].v.fv = 1;
									jarray[n].prim_type	 = PT_BOOL;
									break;

								case 'F':
								case 'f':
									jarray[n].prim_type	 = PT_BOOL;
									jarray[n].v.fv = 0;
									break;

								case 'N':										// assume null, nil, or some variant
								case 'n':
									jarray[n].prim_type	 = PT_NULL;
									jarray[n].v.fv = 0;
									break;

								default:
									jarray[n].prim_type	 = PT_VALUE;
									jarray[n].v.fv = strtof( data, NULL );		// store all numerics as float
									break;
							}

							jarray[n].jsmn_type = JSMN_PRIMITIVE;
							break;

						case JSMN_UNDEFINED:
							// fallthrough
						default:
							fprintf( stderr, "[WARN] jwrapper: [%d] array element %d is not valid type (unknown=%d) is not string or primative\n", i, n, jtokens[i].type  );
							rmr_sym_free( st );
							return NULL;
							break;
					}
				}

				i += size - 1;		// must allow loop to push to next
				break;

			case JSMN_STRING:
				data = extract( json, &jtokens[i] );
				if( (jtp = mk_thing( st, name, jtokens[i].type )) != NULL ) {
					jtp->prim_type = PT_STRING;
					jtp->v.pv =  (void *) data;						// just point into the large json string
				}
				break;

			case JSMN_PRIMITIVE:
				data = extract( json, &jtokens[i] );
				if( (jtp = mk_thing( st, name, jtokens[i].type )) != NULL ) {
					switch( *data ) {								// assume T|t is true and F|f is false
						case 'T':
						case 't':
							jtp->prim_type = PT_BOOL;
							jtp->v.fv = 1;
							break;

						case 'F':
						case 'f':
							jtp->prim_type = PT_BOOL;
							jtp->v.fv = 0;
							break;

						case 'N':									// Null or some form of that
						case 'n':
							jtp->prim_type = PT_NULL;
							jtp->v.fv = 0;
							break;

						default:
							jtp->prim_type = PT_VALUE;
							jtp->v.fv = strtof( data, NULL );		// store all numerics as float
							break;
					}
				}
				break;

			default:
				fprintf( stderr, "[WARN] jwrapper: element [%d] is undefined or of unknown type\n", i );
				break;
		}
	}

	free( jtokens );
	return st;
}

// --------------- public functions -----------------------------------------------------------------

/*
	Destroy all operating structures assocaited with the symtab pointer passed in.
*/
extern void jw_nuke( void* st ) {
	char*	buf;					// pointer to the original json to free

	if( st != NULL ) {
		rmr_sym_foreach_class( st, OBJ_SPACE, nix_things, NULL );	// free anything that the symtab references
		rmr_sym_free( st );											// free the symtab itself
	}
}

/*
	Scan the given st and write some useful (?) info to stderr.
*/
extern void jw_dump( void* st ) {
	if( st != NULL ) {
		rmr_sym_foreach_class( st, OBJ_SPACE, dump_things, NULL );
	} else {
		fprintf( stderr, "<DBUG> jwrapper: dump: no table\n" );
	}
}

/*
	Given a json string, parse it, and put the things into a symtab.
	return the symtab pointer to the caller. They pass the symtab
	pointer back to the various get functions.

	This is the entry point. It sets up the symbol table and invokes the parse object
	funtion to start at the first level. Parse object will recurse for nested objects
	if present.
*/
extern void* jw_new( const char* json ) {
	void	*st;				// symbol table
	char*	djson;				// dup so we can save it
	void*	rp = NULL;			// return value

	if( json != NULL && (st = rmr_sym_alloc( MAX_THINGS )) != NULL ) {
		djson = strdup( json );													// allows user to free/overlay their buffer as needed
		rmr_sym_put( st, (unsigned char *) JSON_SYM_NAME, OBJ_SPACE, djson );	// must have a reference to the string until symtab is trashed

		rp =  parse_jobject( st,  djson, "" );									// empty prefix for the root object
	}

	return rp;
}

/*
	Returns true (1) if the named field is missing.
*/
extern int jw_missing( void* st, const char* name ) {
	int rv = 0;

	if( st != NULL && name != NULL ) {
		rv = rmr_sym_get( st, name, OBJ_SPACE ) == NULL;
	}

	return rv;
}

/*
	Returns true (1) if the named field is in the blob;
*/
extern int jw_exists( void* st, const char* name ) {
	int rv = 0;

	if( st != NULL && name != NULL ) {
		rv =  rmr_sym_get( st, name, OBJ_SPACE ) != NULL;
	}

	return rv;
}

/*
	Returns true (1) if the primative type is value (float).
*/
extern int jw_is_value( void* st, const char* name ) {
	jthing_t* jtp;									// thing that is referenced by the symtab
	int rv = 0;

	if( st != NULL &&
		name != NULL &&
		(jtp = (jthing_t *) rmr_sym_get( st, name, OBJ_SPACE )) != NULL ) {

		rv = jtp->prim_type == PT_VALUE;
	}

	return rv;
}
/*
	Returns true (1) if the primative type is string.
*/
extern int jw_is_string( void* st, const char* name ) {
	jthing_t* jtp;									// thing that is referenced by the symtab
	int rv = 0;

	if( st != NULL &&
		name != NULL &&
		(jtp = (jthing_t *) rmr_sym_get( st, name, OBJ_SPACE )) != NULL ) {

		rv = jtp->prim_type == PT_STRING;
	}

	return rv;
}

/*
	Returns true (1) if the primative type is boolean.
*/
extern int jw_is_bool( void* st, const char* name ) {
	jthing_t* jtp;									// thing that is referenced by the symtab
	int		rv = 0;

	if( st != NULL &&
		name != NULL &&
		(jtp = (jthing_t *) rmr_sym_get( st, name, OBJ_SPACE )) != NULL ) {

		rv =  jtp->prim_type == PT_BOOL;
	}

	return rv;
}

/*
	Returns true (1) if the primative type was a 'null' type.
*/
extern int jw_is_null( void* st, const char* name ) {
	jthing_t* jtp;									// thing that is referenced by the symtab
	int		rv = 0;

	if( st != NULL &&
		name != NULL &&
		(jtp = (jthing_t *) rmr_sym_get( st, name, OBJ_SPACE )) != NULL ) {

		rv = jtp->prim_type == PT_NULL;
	}

	return rv;
}

/*
	Look up the name in the symtab and return the string (data).
*/
extern char* jw_string( void* st, const char* name ) {
	jthing_t* jtp;									// thing that is referenced by the symtab
	char*		rv = NULL;

	if( st != NULL &&
		name != NULL &&
		(jtp = (jthing_t *) rmr_sym_get( st, name, OBJ_SPACE )) != NULL ) {

		if( jtp->jsmn_type == JSMN_STRING ) {
			rv = (char *) jtp->v.pv;
		}
	}

	return rv;
}

/*
	Look up name and return the value.
*/
extern float jw_value( void* st, const char* name ) {
	jthing_t* jtp;									// thing that is referenced by the symtab
	float	rv = 0.0;

	if( st != NULL &&
		name != NULL &&
		(jtp = (jthing_t *) rmr_sym_get( st, name, OBJ_SPACE )) != NULL ) {

		if( jtp->jsmn_type == JSMN_PRIMITIVE ) {
			rv = jtp->v.fv;
		}
	}

	return rv;
}

/*
	Look up name and return the blob (symtab).
*/
extern void* jw_blob( void* st, const char* name ) {
	jthing_t* jtp;									// thing that is referenced by the symtab
	void*	rv = NULL;

	if( st != NULL &&
		name != NULL &&
		(jtp = (jthing_t *) rmr_sym_get( st, name, OBJ_SPACE )) != NULL ) {

		if( jtp->jsmn_type == JSMN_OBJECT ) {
			rv = (void *) jtp->v.pv;
		}
	}

	return rv;
}

/*
	Look up the element and return boolean state; This takes the C approach and
	returns true/false based on the value.
*/
extern int jw_bool_ele( void* st, const char* name, int idx ) {
	jthing_t* jtp;									// thing that is referenced by the symtab entry
	int		rv = 0;

	if( st != NULL &&
		name != NULL &&
		(jtp = suss_element( st, name, idx )) != NULL ) {

			rv = !! ((int) jtp->v.fv);
	}

	return rv;
}
/*
	Look up array element as a string. Returns NULL if:
		name is not an array
		name is not in the hash
		index is out of range
		element is not a string
*/
extern char* jw_string_ele( void* st, const char* name, int idx ) {
	jthing_t* jtp;									// thing that is referenced by the symtab entry
	char*		rv = NULL;

	if( st != NULL &&
		name != NULL &&
		(jtp = suss_element( st, name, idx )) != NULL ) {

		if( jtp->jsmn_type == JSMN_STRING ) {
			rv = (char *) jtp->v.pv;
		}
	}

	return rv;
}

/*
	Look up array element as a value. Returns 0 if:
		name is not an array
		name is not in the hash
		index is out of range
		element is not a value
*/
extern float jw_value_ele( void* st, const char* name, int idx ) {
	jthing_t*	jtp;							// thing that is referenced by the symtab entry
	float		rv = 0.0;

	if( st != NULL &&
		name != NULL &&
		(jtp = suss_element( st, name, idx )) != NULL ) {

		if( jtp->prim_type == PT_VALUE ) {
			rv = jtp->v.fv;
		}
	}

	return rv;
}
/*
	Look up the element and check to see if it is a string.
	Return true (1) if it is.
*/
extern int jw_is_string_ele( void* st, const char* name, int idx ) {
	jthing_t* jtp;									// thing that is referenced by the symtab entry
	int		rv = 0;

	if( st != NULL &&
		name != NULL &&
		(jtp = suss_element( st, name, idx )) != NULL ) {

			rv = jtp->prim_type == PT_STRING;
	}

	return rv;
}

/*
	Look up the element and check to see if it is a value primative.
	Return true (1) if it is.
*/
extern int jw_is_value_ele( void* st, const char* name, int idx ) {
	jthing_t* jtp;									// thing that is referenced by the symtab entry
	int		rv = 0;

	if( st != NULL &&
		name != NULL &&
		(jtp = suss_element( st, name, idx )) != NULL ) {

			rv = jtp->prim_type == PT_VALUE;
	}

	return rv;
}

/*
	Look up the element and check to see if it is a boolean primative.
	Return true (1) if it is.
*/
extern int jw_is_bool_ele( void* st, const char* name, int idx ) {
	jthing_t* jtp;									// thing that is referenced by the symtab entry
	int		rv = 0;

	if( st != NULL &&
		name != NULL &&
		(jtp = suss_element( st, name, idx )) != NULL ) {

			rv = jtp->prim_type == PT_BOOL;
	}

	return rv;
}

/*
	Look up the element and check to see if it is a null primative.
	Return true (1) if it is.
*/
extern int jw_is_null_ele( void* st, const char* name, int idx ) {
	jthing_t* jtp;									// thing that is referenced by the symtab entry
	int		rv = 0;

	if( st != NULL &&
		name != NULL &&
		(jtp = suss_element( st, name, idx )) != NULL ) {

			rv =  jtp->prim_type == PT_NULL;
	}

	return rv;
}

/*
	Look up array element as an object. Returns NULL if:
		name is not an array
		name is not in the hash
		index is out of range
		element is not an object

	An object in an array is a standalone symbol table. Thus the object
	is treated differently than a nested object whose members are a
	part of the parent namespace.  An object in an array has its own
	namespace.
*/
extern void* jw_obj_ele( void* st, const char* name, int idx ) {
	jthing_t* jtp;									// thing that is referenced by the symtab entry
	void*		rv = NULL;

	if( st != NULL &&
		name != NULL &&
		(jtp = suss_element( st, name, idx )) != NULL ) {

		if( jtp->jsmn_type == JSMN_OBJECT ) {
			rv = (void *) jtp->v.pv;
		}
	}

	return rv;
}

/*
	Return the size of the array named. Returns -1 if the thing isn't an array,
	and returns the number of elements otherwise.
*/
extern int jw_array_len( void* st, const char* name ) {
	jthing_t* jtp;									// thing that is referenced by the symtab entry
	int		rv = -1;

	if( st != NULL &&
		name != NULL &&
		(jtp = suss_array( st, name )) != NULL ) {

		rv = jtp->nele;
	}

	return rv;
}
