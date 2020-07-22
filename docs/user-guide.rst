============
USER'S GUIDE
============
.. This work is licensed under a Creative Commons Attribution 4.0 International License.
.. SPDX-License-Identifier: CC-BY-4.0
..
.. CAUTION: this document is generated from source in doc/src/*
.. To make changes edit the source and recompile the document.
.. Do NOT make changes directly to .rst or .md files.




INTRODUCTION
============

The C++ framework allows the programmer to create an xApp
object instance, and to use that instance as the logic base.
The xApp object provides a message level interface to the RIC
Message Router (RMR), including the ability to register
callback functions which the instance will drive as messages
are received; much in the same way that an X-windows
application is driven by the window manager for all activity.
The xApp may also choose to use its own send/receive loop,
and thus is not required to use the callback driver mechanism
provided by the framework.


THE FRAMEWORK API
=================

The C++ framework API consists of the creation of the xApp
object, and invoking desired functions via the instance of
the object. The following paragraphs cover the various steps
involved to create an xApp instance, wait for a route table
to arrive, send a message, and wait for messages to arrive.


The Namespace
-------------

Starting with version 2.0.0 the framwork introduces a
*namespace* of ``xapp`` for the following classes and types:


   * Alarm
   * Jhash
   * Message
   * Msg_component


This is a breaking change and as such the major version was
bumpped from 1 to 2.


Creating the xApp instance
--------------------------

The creation of the xApp instance is as simple as invoking
the object's constructor with two required parameters:


       .. list-table::
         :widths: auto
         :header-rows: 0
         :class: borderless

         * - **port**
           -
             A C string (pointer to char) which defines the port that
             RMR will open to listen for connections.


             |

         * - **wait**
           -
             A Boolean value which indicates whether or not the
             initialization process should wait for the arrival of a
             valid route table before completing. When true is
             supplied, the initialization will not complete until RMR
             has received a valid route table (or one is located via
             the ``RMR_SEED_RT`` environment variable).



The following code sample illustrates the simplicity of
creating the instance of the xApp object.


::

      #include <memory>
      #include <ricxfcpp/xapp.hpp>
      int main( ) {
          std::unique_ptr<Xapp> xapp;
          char* listen_port = (char *) "4560";    //RMR listen port
          bool  wait4table = true;            // wait for a route table

          xapp = std::unique_ptr<Xapp>(
                new Xapp( listen_port, wait4table ) );
      }

Figure 1: Creating an xAPP instance.

From a compilation perspective, the following is the simple
compiler invocation string needed to compile and link the
above program (assuming that the sample code exists in a file
called ``man_ex1.cpp``.


::

     g++ man_ex1.cpp -o man_ex1 -lricxfcpp -lrmr_si -lpthread


The above program, while complete and capable of being
compiled, does nothing useful. When invoked, RMR will be
initialized and will begin listening for a route table;
blocking the return to the main program until one is
received. When a valid route table arrives, initialization
will complete and the program will exit as there is no code
following the instruction to create the object.


LISTENING FOR MESSAGES
======================

The program in the previous example can be extended with just
a few lines of code to enable it to receive and process
messages. The application needs to register a callback
function for each message type which it desires to process.

Once registered, each time a message is received the
registered callback for the message type will be invoked by
the framework.


Callback Signature
------------------

As with most callback related systems, a callback must have a
well known function signature which generally passes event
related information and a "user" data pointer which was
registered with the function. The following is the prototype
which callback functions must be defined with:


::

      void cb_name( xapp::Message& m, int mtype, int subid,
            int payload_len, xapp::Msg_component payload,
            void* usr_data );

Figure 2: Callback function signature

The parameters passed to the callback function are as
follows:


       .. list-table::
         :widths: auto
         :header-rows: 0
         :class: borderless

         * - **m**
           -
             A reference to the Message that was received.


             |

         * - **mtype**
           -
             The message type (allows for disambiguation if the
             callback is registered for multiple message types).


             |

         * - **subid**
           -
             The subscription ID from the message.


             |

         * - **payload len**
           -
             The number of bytes which the sender has placed into the
             payload.


             |

         * - **payload**
           -
             A direct reference (smart pointer) to the payload. (The
             smart pointer is wrapped in a special class in order to
             provide a custom destruction function without burdening
             the xApp developer with that knowledge.)


             |

         * - **user data**
           -
             A pointer to user data. This is the pointer that was
             provided when the function was registered.



To illustrate the use of a callback function, the previous
code example has been extended to add the function, register
it for message types 1000 and 1001, and to invoke the
``Run()`` function in the framework (explained in the next
section).

::

      #include <memory>
      #include <ricxfcpp/xapp.hpp>
      long m1000_count = 0;    // message counters, one for each type
      long m1001_count = 0;

      // callback function that will increase the appropriate counter
      void cbf( xapp::Message& mbuf, int mtype, int subid, int len,
                  xapp::Msg_component payload,  void* data ) {
          long* counter;

          if( (counter = (long *) data) != NULL ) {
              (*counter)++;
          }
      }

      int main( ) {
          std::unique_ptr<Xapp> xapp;
          char* listen_port = (char *) "4560";
          bool  wait4table = false;

          xapp = std::unique_ptr<Xapp>(
                new Xapp( listen_port, wait4table ) );

          // register the same callback function for both msg types
          xapp->Add_msg_cb( 1000, cbf, (void *) &m1000_count );
          xapp->Add_msg_cb( 1001, cbf, (void *) &m1001_count );

          xapp->Run( 1 );        // start the callback driver
      }

Figure 3: Callback function example.

As before, the program does nothing useful, but now it will
execute and receive messages. For this example, the same
function can be used to increment the appropriate counter
simply by providing a pointer to the counter as the user data
when the callback function is registered. In addition, a
subtle change from the previous example has been to set the
wait for table flag to ``false.``

For an xApp that is a receive only application (never sends)
it is not necessary to wait for RMR to receive a table from
the Route Manager.


Registering A Default Callback
------------------------------

The xApp may also register a default callback function such
that the function will be invoked for any message that does
not have a registered callback. If the xAPP does not register
a default callback, any message which cannot be mapped to a
known callback function is silently dropped. A default
callback is registered by providing a *generic* message type
of ``xapp->DEFAULT_CALLBACK`` on an ``Add_msg_cb`` call.


The Framework Callback Driver
-----------------------------

The ``Run()`` function within the Xapp object is invoked to
start the callback driver, and the xApp should not expect the
function to return under most circumstances. The only
parameter that the ``Run()`` function expects is the number
of threads to start. For each thread requested, the framework
will start a listener thread which will allow received
messages to be processed in parallel. If supplying a value
greater than one, the xApp must ensure that the callback
functions are thread safe as it is very likely that the same
callback function will be invoked concurrently from multiple
threads.


SENDING MESSAGES
================

It is very likely that most xApps will need to send messages
and will not operate in "receive only" mode. Sending the
message is a function of the message object itself and can
take one of two forms:


   * Replying to the sender of a received message

   * Sending a message (routed based on the message type and
     subscription ID)


When replying to the sender, the message type and
subscription ID are not used to determine the destination of
the message; RMR ensures that the message is sent back to the
originating xApp. The xApp may still need to change the
message type and/or the subscription ID in the message prior
to using the reply function.

To provide for both situations, two reply functions are
supported by the Message object as illustrated with the
following prototypes.


::

     bool Send_response(  int mtype, int subid, int response_len,
          std:shared_ptr<unsigned char> response );

     bool Send_response(  int response_len, std::shared_ptr<unsigned char> response );

Figure 4: Reply function prototypes.

In the first prototype the xApp must supply the new message
type and subscription ID values, where the second function
uses the values which are currently set in the message.
Further, the new payload contents, and length, are supplied
to both functions; the framework ensures that the message is
large enough to accommodate the payload, reallocating it if
necessary, and copies the response into the message payload
prior to sending. Should the xApp need to change either the
message type, or the subscription ID, but not both, the
``NO_CHANGE`` constant can be used as illustrated below.


::

      msg->Send_response( xapp::Message::NO_CHANGE, xapp::Message::NO_SUBID,
          pl_length, (unsigned char *) payload );

Figure 5: Send response prototype.

In addition to the two function prototypes for
``Send_response()`` there are two additional prototypes which
allow the new payload to be supplied as a shared smart
pointer. The other parameters to these functions are
identical to those illustrated above, and thus are not
presented here.

The ``Send_msg()`` set of functions supported by the Message
object are identical to the ``Send_response()`` functions and
are shown below.


::

      bool Send_msg( int mtype, int subid, int payload_len,
          std::shared_ptr<unsigned char> payload );

      bool Send_msg( int mtype, int subid, int payload_len,
          unsigned char* payload );

      bool Send_msg( int payload_len,
          std::shared_ptr<unsigned char> payload );

      bool Send_msg( int payload_len, unsigned char* payload );

Figure 6: Send function prototypes.

Each send function accepts the message, copies in the payload
provided, sets the message type and subscription ID (if
provided), and then causes the message to be sent. The only
difference between the ``Send_msg()`` and
``Send_response()`` functions is that the destination of the
message is selected based on the mapping of the message type
and subscription ID using the current routing table known to
RMR.


Direct Payload Manipulation
---------------------------

For some applications, it might be more efficient to
manipulate the payload portion of an Xapp Message in place,
rather than creating it and relying on a buffer copy when the
message is finally sent. To achieve this, the xApp must
either use the smart pointer to the payload passed to the
callback function, or retrieve one from the message using
``Get_payload()`` when working with a message outside of a
callback function. Once the smart pointer is obtained, the
pointer's get() function can be used to directly reference
the payload (unsigned char) bytes.

When working directly with the payload, the xApp must take
care not to write more than the actual payload size which can
be extracted from the Message object using the
``Get_available_size()`` function.

When sending a message where the payload has been directly
altered, and no extra buffer copy is needed, a NULL pointer
should be passed to the Message send function. The following
illustrates how the payload can be directly manipulated and
returned to the sender (for simplicity, there is no error
handling if the payload size of the received message isn't
large enough for the response string, the response is just
not sent).


::

      Msg_component payload;  // smart reference
      int pl_size;            // max size of payload

      payload = msg->Get_payload();
      pl_size = msg->Get_available_size();
      if( snprintf( (char *) payload.get(), pl_size,
          "Msg Received\\n" ) < pl_size ) {
        msg->Send_response( M_TYPE, SID, strlen( raw_pl ), NULL );
      }

Figure 7: Send message without buffer copy.



Sending Multiple Responses
--------------------------

It is likely that the xApp will wish to send multiple
responses back to the process that sent a message that
triggered the callback. The callback function may invoke the
``Send_response()`` function multiple times before returning.

After each call, the Message retains the necessary
information to allow for a subsequent invocation to send more
data. It should be noted though, that after the first call to
``{Send_response()`` the original payload will be lost; if
necessary, the xApp must make a copy of the payload before
the first response call is made.


Message Allocation
------------------

Not all xApps will be "responders," meaning that some xApps
will need to send one or more messages before they can expect
to receive any messages back. To accomplish this, the xApp
must first allocate a message buffer, optionally initialising
the payload, and then using the message's ``Send_msg()``
function to send a message out. The framework's
``Alloc_msg()`` function can be used to create a Message
object with a desired payload size.


FRAMEWORK PROVIDED CALLBACKS
============================

The framework itself may provide message handling via the
driver such that the xApp might not need to implement some
message processing functionality. Initially, the C++
framework will provide a default callback function to handle
the RMR based health check messages. This callback function
will assume that if the message was received, and the
callback invoked, that all is well and will reply with an OK
state. If the xApp should need to override this simplistic
response, all it needs to do is to register its own callback
function for the health check message type.


JSON SUPPORT
============

The C++ xAPP framework provides a very lightweight json
parser and data hash facility. Briefly, a json hash (Jhash)
can be established by creating an instance of the Jhash
object with a string of valid json. The resulting object's
functions can then be used to read values from the resulting
hash.


Creating The Jhash Object
-------------------------

The Jhash object is created simply by passing a json string
to the constructor.

::

      #include <ricxfcpp/Jhash>

      std::string jstring = "{ \\"tag\\": \\"Hello World\\" }";
      Jhash*  jh;

      jh =  new Jhash( jstring.c_str() );

Figure 8: The creation of the Jhash object.

Once the Jhash object has been created any of the methods
described in the following paragraphs can be used to retrieve
the data:


Json Blobs
----------

Json objects can be nested, and the nesting is supported by
this representation. The approach taken by Jhash is a
"directory view" approach, where the "current directory," or
current *blob,* limits the scope of visible fields.

As an example, the json contained in figure 9, contains a
"root" blob and two *sub-blobs* (address and lease_info).


::

      {
          "lodge_name": "Water Buffalo Lodge 714",
          "member_count": 41,
          "grand_poobah": "Larry K. Slate",
          "attendance":   [ 23, 14, 41, 38, 24 ],
          "address": {
              "street":    "16801 Stonway Lane",
              "suite":     null,
              "city":      "Bedrock",
              "post_code": "45701"
          },
          "lease_info": {
              "owner":    "Stonegate Properties",
              "amount":   216.49,
              "due":      "monthly",
              "contact:"  "Kyle Limestone"
          }
      }

Figure 9: Sample json with a root and two blobs.

Upon creation of the Jhash object, the *root* fields,
``lodge_name,`` ``member_count,`` and ``grand_poobah`` are
immediately available. The fields in the *sub-blobs* are
available only when the correct blob is selected. The code
sample in figure 10 illustrates how a *sub-blob* is selected.

::

      jh->Set_blob( (char *) "address" );     // select address
      jh->Unset_blob();                       // return to root
      jh->Set_blob( (char *) "lease_info" );  // select the lease blob

Figure 10: Blob selection example.

Currently, the selected blob must be unset in order to select
a blob at the root level; unset always sets the root blob.
Attempting to use the ``Set_blob`` function will attempt to
select the named blob from the current blob, and not the
root.


Simple Value Extraction
-----------------------

Simple values are the expected data types *string, value,*
and *boolean.* This lightweight json parser treats all values
as floating point numbers and does not attempt to maintain a
separate integer type. A fourth type, *null,* is supported to
allow the user to expressly check for a field which is
defined but has no value; as opposed to a field that was
completely missing from the data. The following are the
prototypes for the functions which allow values to be
extracted:


::

      std::string String( const char* name );
      float Value( const char* name );
      bool Bool( const char* name );


Each of these functions returns the value associated with the
field with the given *name.* If the value is missing, the
following default values are returned:


       .. list-table::
         :widths: 15,80
         :header-rows: 0
         :class: borderless

         * - **String**
           -
             An empty string (.e.g "").

             |

         * - **Value**
           -
             Zero (e.g 0.0)

             |

         * - **bool**
           -
             false



If the user needs to disambiguate between a missing value and
the default value either the ``Missing`` or ``Exists``
function should be used first.


Testing For Existing and Missing Fields
---------------------------------------

Two functions allow the developer to determine whether or not
a field is included in the json. Both of these functions work
on the current *blob,* therefore it is important to ensure
that the correct blob is selected before using either of
these functions. The prototypes for the ``Exists`` and
``Missing`` functions are below:

::

      bool Exists( const char* name );
      bool Is_missing( const char* name );

The ``Exists`` function returns *true* if the field name
exists in the json and *false* otherwise. Conversely, the
``Missing`` function returns *true* when the field name does
not exist in the json.


Testing Field Type
------------------

The ``Exists`` and ``Missing`` functions might not be enough
for the user code to validate the data that it has. To assist
with this, several functions allow direct type testing on a
field in the current blob. The following are the prototypes
for these functions:

::

      bool Is_bool( const char* name );
      bool Is_null( const char* name );
      bool Is_string( const char* name );
      bool Is_value( const char* name );


Each of these functions return *true* if the field with the
given name is of the type being tested for.


Arrays
------

Arrays are supported in the same manner as simple field
values with the addition of the need to supply an array index
when fetching values from the object. In addition, there is a
*length* function which can be used to determine the number
of elements in the named array. The prototypes for the array
based functions are below:

::

      int Array_len( const char* name );

      bool Is_bool_ele( const char* name, int eidx );
      bool Is_null_ele( const char* name, int eidx );
      bool Is_string_ele( const char* name, int eidx );
      bool Is_value_ele( const char* name, int eidx );

      bool Bool_ele( const char* name, int eidx );
      std::string String_ele( const char* name, int eidx );
      float Value_ele( const char* name, int eidx );


For each of these functions the ``eidx`` is the zero based
element index which is to be tested or selected.


Arrays of Blobs
---------------

An array containing blobs, rather than simple field value
pairs, the blob must be selected prior to using it, just as a
sub-blob needed to be selected. The ``Set_blob_ele`` function
is used to do this and has the following prototype:

::

      bool Set_blob_ele( const char* name, int eidx );


As with selecting a sub-blob, an unset must be performed
before selecting the next blob. Figure 11 illustrates how
these functions can be used to read and print values from the
json in figure 12.

::

      "members": [
          { "name": "Fred Flinstone", "member_num": 42 },
          { "name": "Barney Rubble", "member_num": 48 },
          { "name": "Larry K Slate", "member_num": 22 },
          { "name": "Kyle Limestone", "member_num": 49 }
      ]

Figure 11: Json array containing blobs.


::

      std::string mname;
      float mnum;
      int len;

      len = jh->Array_len( (char *) "members" );
      for( i = 0; i < len; i++ ) {
          jh->Set_blob_ele( (char *) "members", i );  // select blob

          mname = jh->String( (char *) "name" );      // read values
          mnum = jh->Value( (char *) "member_num" );
          fprintf( stdout, "%s is member %d\\n", mname.c_str(), (int) mnum );

          jh->Unset_blob();                           // back to root
      }

Figure 12: Code to process the array of blobs.



ALARM MANAGER INTERFACE
=======================

The C++ framework provides an API which allows the xAPP to
easily construct and generate alarm messages. Alarm messages
are a special class of RMR message, allocated in a similar
fashion as an RMR message through the framework's
``Alloc_alarm()`` function.

The API consists of the following function types:


       .. list-table::
         :widths: auto
         :header-rows: 0
         :class: borderless

         * - **Raise**
           -
             Cause the alarm to be assigned a severity and and sent via
             RMR message to the alarm collector process.


             |

         * - **Clear**
           -
             Cause a clear message to be sent to the alarm collector.


             |

         * - **Raise Again**
           -
             Cause a clear followed by a raise message to be sent to
             the alarm collector.





Allocating Alarms
-----------------

The ``xapp`` function provided by the framework is used to
create an alarm object. Once the xAPP has an alarm object it
can be used to send one, or more, alarm messages to the
collector.

The allocation function has three prototypes which allow the
xAPP to create an alarm with an initial set of information as
is appropriate. The following are the prototypes for the
allocate functions:


::

    std::unique_ptr<xapp::Alarm> Alloc_alarm( );
    std::unique_ptr<xapp::Alarm> Alloc_alarm( std::string meid );
    std::unique_ptr<xapp::Alarm> Alloc_alarm( int prob_id, std::string meid );

Figure 13: Alarm allocation prototypes.

Each of the allocation functions returns a unique pointer to
the alarm. In the simplest form (1) the alarm is initialised
with an empty meid (managed element ID) string, and unset
problem ID (-1). The second prototype allows the xAPP to
supply the meid, and in the third form both the problem ID
and the meid are used to initialise the alarm.


Raising An Alarm
----------------

Once an alarm has been allocated, its ``Raise()`` function
can be used to cause the alarm to be sent to the collector.
The raise process allows the xAPP to perform the following
modifications to the alarm before sending the message:


   * Set the alarm severity

   * Set the problem ID value

   * Set the alarm information string

   * Set the additional information string


The following are the prototypes for the ``Raise()``
functions of an alarm object: ..... In its simplest form (1)
the ``Raise()`` function will send the alarm without making
any changes to the data. The final two forms allow the xAPP
to supply additional data which is added to the alarm before
sending the message. Each of the raise functions returns
``true`` on success and ``false`` if the alarm message could
not be sent.


Severity
--------

The severity is one of the ``SEV_`` constants listed below.
These map to alarm collector strings and insulate the xAPP
from any future alarm collector changes. The specific meaning
of these severity types are defined by the alarm collector
and thus no attempt is made to guess what their actual
meaning is. These constants are available by including
``alarm.hpp.``


   ::

         SEV_MAJOR
         SEV_MINOR
         SEV_WARN
         SEV_DEFAULT

Figure 14: Severity constants available in alarm.hpp.


The Problem ID
--------------

The problem ID is an integer which is assigned by the xAPP.
The framework makes no attempt to verify that it has been se,
nor does it attempt to validate the value. If the xAPP does
not set the value, ``-1`` is used.


Information Strings
-------------------

The two information strings are also xAPP defined and provide
the information that the xAPP deems necessary and related to
the alarm. What the collector expects, and how these strings
are used, is beyond the scope of the framework to describe or
validate. If not supplied, empty strings are sent in the
alarm message.


Clearing An Alarm
-----------------

The ``Clear()`` function of an alarm may be used to send a
clear message. In a manner similar to the ``Raise()``
functions, the ``Clear()`` functions allow the existing alarm
data to be sent without change, or for the xAPP to modify the
data before the message is sent to the collector. The
following are the prototype for these functions.

::

     bool Clear( );
     bool Clear( int severity, int problem, std::string info );
     bool Clear( int severity, int problem, std::string info, std::string addional_info );
     bool Clear_all( );


Figure 15: Clear function prototypes.

Each of the clear functions returns ``true`` on success and
``false`` if the alarm message could not be sent.

The ``Clear_all()`` function sends a special action code to
the collector which is assumed to clear all alarms. However,
it is unknown whether that implies **all** alarms, or all
alarms matching the ``problem_id,`` or some other
interpretation. Please consult the alarm collector
documentation for these specifics.


Adjusting Alarm Contents
------------------------

It might be necessary for the xAPP to adjust the alarm
contents outside of the scope of the ``Raise()`` function, or
to adjust data that cannot be manipulated by ``Raise().`` The
following are the (self explanatory) prototypes for the
*setter* functions which are available to the xAPP.


::

    void Set_additional( std::string new_info );
    void Set_appid( std::string new_id );
    void Set_info( std::string new_info );
    void Set_meid( std::string new_meid );
    void Set_problem( int new_id );
    void Set_severity( int new_sev );

Figure 16: Alarm Setters



METRICS SUPPORT
===============

The C++ xAPP framework provides a lightweight interface to
the metrics gateway allowing the xAPP to create and send
metrics updates without needing to understand the underlying
message format. From the xAPP's perspective, the metrics
object is created with one or more key/value measurement
pairs and then is sent to the process responsible for
forwarding them to the various collection processes. The
following sections describe the Metrics object and the API
associated with it.


Creating The Metrics Object
---------------------------

The ``xapp`` object can be created directly, or via the xapp
framework. When creating directly the xAPP must supply an RMR
message for the object to use; when the framework is used to
create the object, the message is created as as part of the
process. The framework provides three constructors for the
metrics instance allowing the xAPP to supply the measurement
source, the source and reporter, or to default to using the
xAPP name as both the source and reporter (see section
*Source and Reporter* for a more detailed description of
these). The framework constructors are illustrated in figure
17.


::

    std::unique_ptr<xapp::Metrics> Alloc_metrics( );
    std::unique_ptr<xapp::Metrics> Alloc_metrics( std::string source );
    std::unique_ptr<xapp::Metrics> Alloc_metrics( std::string reporter, std::string source );

Figure 17: The framework constructors for creating an
instance of the metrics object.


::


      #include <ricxfcpp/Metrics>

      char* port = (char *) "4560";

      auto x = std::unique_ptr<Xapp>( new Xapp( port ) );
      auto reading = std::shared_ptr<xapp::Metrics>( x->Alloc_metric( ) );

Figure 18: Metrics instance creation using the framework.

Figures 18 illustrates how the framework constructor can be
used to create a metrics instance. While it is unlikely that
an xAPP will create a metrics instance directly, there are
three similar constructors available. These are prototypes
are shown in figure 19 and their use is illustrated in figure
20.

::

     Metrics( std::shared_ptr<xapp::Message> msg );
     Metrics( std::shared_ptr<xapp::Message> msg, std::string msource );
     Metrics( std::shared_ptr<xapp::Message> msg, std::string reporter, std::string msource );

Figure 19: Metrics object constructors.


::

      #include <ricxfcpp/Metrics>

      char* port = (char *) "4560";

      auto x = std::unique_ptr<Xapp>( new Xapp( port ) );
      auto msg = std::shared_ptr<xapp::Message>( x->Alloc_msg( 4096 ) );
      auto reading = std::shared_ptr<xapp::Metrics>( new Metrics( msg ) );

Figure 20: Direct creation of a metrics instance.



Adding Values
-------------

Once an instance of the metrics object is created, the xAPP
may push values in preparation to sending the measurement(s)
to the collector. The ``Push_data()`` function is used to
push each key/value pair and is illustrated in figure 21.

::

          reading->Push_data( "normal_count", (double) norm_count );
          reading->Push_data( "high_count", (double) hi_count );
          reading->Push_data( "excessive_count", (double) ex_count );

Figure 21: Pushing key/value pairs into a metrics instance.



Sending A Measurement Set
-------------------------

After all of the measurement key/value pairs have been added
to the instance, the ``Send()`` function can be invoked to
create the necessary RMR message and send that to the
collection application. Following the send, the key/value
pairs are cleared from the instance and the xAPP is free to
start pushing values into the instance again. The send
function has the following prototype and returns ``true`` on
success and ``false`` if the measurements could not be sent.


Source and Reporter
-------------------

The alarm collector has the understanding that a measurement
might be *sourced* from one piece of equipment, or software
component, but reported by another. For auditing purposes it
makes sense to distinguish these, and as such the metrics
object allows the xAPP to identify the case when the source
and reporter are something other than the xAPP which is
generating the metrics message(s).

The *source* is the component to which the measurement
applies. This could be a network interface card counting
packets, a temperature sensor, or the xAPP itself reporting
xAPP related metrics. The *reporter* is the application that
is reporting the measurement(s) to the collector.

By default, both reporter and source are assumed to be the
xAPP, and the name is automatically determined using the
run-time supplied programme name. Should the xAPP need to
report measurements for more than one source it has the
option to create an instance for every reporter source
combination, or to set the reporter and/or source with the
generation of each measurement set. To facilitate the ability
to change the source and/or the reporter without the need to
create a new metrics instance, two *setter* functions are
provided. The prototypes for these are shown in figure 22.


::

      void Set_source( std::string new_source );
      void Set_reporter( std::string new_reporter );

Figure 22: Setter functions allowing the reporter and/or
source to be set after construction.

