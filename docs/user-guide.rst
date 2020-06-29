.. This work is licensed under a Creative Commons Attribution 4.0 International License.
.. SPDX-License-Identifier: CC-BY-4.0
..
.. CAUTION: this document is generated from source in doc/src/*
.. To make changes edit the source and recompile the document.
.. Do NOT make changes directly to .rst or .md files.


============================================================================================
RIC xAPP C++ Framework
============================================================================================
--------------------------------------------------------------------------------------------
User's Guide
--------------------------------------------------------------------------------------------


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


Creating the xApp instance
--------------------------

The creation of the xApp instance is as simple as invoking
the object's constructor with two required parameters:


      port
         A C string (char *) which defines the port that RMR will
         open to listen for connections.

      wait
         A Boolean value which indicates whether or not the
         initialization process should wait for the arrival of a
         valid route table before completing. When true is
         supplied, the initialization will not complete until RMR
         has received a valid route table (or one is located via
         the RMR_SEED_RT environment variable).

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
      called man_ex1.cpp.


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
            void cb_name( Message& m, int mtype, int subid,
                  int payload_len, Msg_component payload,
                  void* usr_data );

      Figure 2: Callback function signature

      The parameters passed to the callback function are as
      follows: &multi_space

      m
         A reference to the Message that was received.

      mtype
         The message type (allows for disambiguation if the
         callback is registered for multiple message types).

      subid
         The subscription ID from the message.

      payload len
         The number of bytes which the sender has placed into the
         payload.

      payload
         A direct reference (smart pointer) to the payload. (The
         smart pointer is wrapped in a special class in order to
         provide a custom destruction function without burdening
         the xApp developer with that knowledge.)

      user data
         A pointer to user data. This is the pointer that was
         provided when the function was registered.

      To illustrate the use of a callback function, the previous
      code example has been extended to add the function, register
      it for message types 1000 and 1001, and to invoke the Run()
      function in the framework (explained in the next section).

      ::
            #include <memory>
            #include <ricxfcpp/xapp.hpp>
            long m1000_count = 0;    // message counters, one for each type
            long m1001_count = 0;

            // callback function that will increase the appropriate counter
            void cbf( Message& mbuf, int mtype, int subid, int len,
                        Msg_component payload,  void* data ) {
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
      wait for table flag to false.

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
      of xapp->DEFAULT_CALLBACK on an Add_msg_cb call.


The Framework Callback Driver
-----------------------------

      The Run() function within the Xapp object is invoked to start
      the callback driver, and the xApp should not expect the
      function to return under most circumstances. The only
      parameter that the Run() function expects is the number of
      threads to start. For each thread requested, the framework
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


      +
      $1 Replying to the sender of a received message

      $1 Sending a message (routed based on the message type and subscription ID)


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
      NO_CHANGE constant can be used as illustrated below.


      ::
            msg->Send_response( Message::NO_CHANGE, Message::NO_SUBID,
                pl_length, (unsigned char *) payload );

      Figure 5: Send response prototype.

      In addition to the two function prototypes for
      Send_response() there are two additional prototypes which
      allow the new payload to be supplied as a shared smart
      pointer. The other parameters to these functions are
      identical to those illustrated above, and thus are not
      presented here.

      The Send_msg() set of functions supported by the Message
      object are identical to the Send_response() functions and are
      shown below.


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
      difference between the Send_msg() and Send_response()
      functions is that the destination of the message is selected
      based on the mapping of the message type and subscription ID
      using the current routing table known to RMR.


Direct Payload Manipulation
---------------------------

      For some applications, it might be more efficient to
      manipulate the payload portion of an Xapp Message in place,
      rather than creating it and relying on a buffer copy when the
      message is finally sent. To achieve this, the xApp must
      either use the smart pointer to the payload passed to the
      callback function, or retrieve one from the message using
      Get_payload() when working with a message outside of a
      callback function. Once the smart pointer is obtained, the
      pointer's get() function can be used to directly reference
      the payload (unsigned char) bytes.

      When working directly with the payload, the xApp must take
      care not to write more than the actual payload size which can
      be extracted from the Message object using the
      Get_available_size() function.

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
      Send_response() function multiple times before returning.

      After each call, the Message retains the necessary
      information to allow for a subsequent invocation to send more
      data. It should be noted though, that after the first call to
      {Send_response() the original payload will be lost; if
      necessary, the xApp must make a copy of the payload before
      the first response call is made.


Message Allocation
------------------

      Not all xApps will be "responders," meaning that some xApps
      will need to send one or more messages before they can expect
      to receive any messages back. To accomplish this, the xApp
      must first allocate a message buffer, optionally initialising
      the payload, and then using the message's Send_msg() function
      to send a message out. The framework's Alloc_msg() function
      can be used to create a Message object with a desired payload
      size.


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


EXAMPLE PROGRAMMES
==================

      The following sections contain several example programmes
      which are written on top of the C++ framework.


RMR Dump xAPP
-------------

      The RMR dump application is an example built on top of the
      C++ xApp framework to both illustrate the use of the
      framework, and to provide a useful diagnostic tool when
      testing and troubleshooting xApps.

      The RMR dump xApp isn't a traditional xApp inasmuch as its
      goal is to listen for message types and to dump information
      about the messages received to the TTY much as tcpdump does
      for raw packet traffic. The full source code, and Makefile,
      are in the examples directory of the C++ framework repo.

      When invoked, the RMR dump program is given one or more
      message types to listen for. A callback function is
      registered for each, and the framework Run() function is
      invoked to drive the process. For each recognised message,
      and depending on the verbosity level supplied at program
      start, information about the received message(s) is written
      to the TTY. If the forwarding option, -f, is given on the
      command line, and an appropriate route table is provided,
      each received message is forwarded without change. This
      allows for the insertion of the RMR dump program into a flow,
      however if the ultimate receiver of a message needs to reply
      to that message, the reply will not reach the original
      sender, so RMR dump is not a complete "middle box"
      application.

      The following is the code for this xAPP. Several functions,
      which provide logic unrelated to the framework, have been
      omitted. The full code is in the framework repository.



      ::
        #include <stdio.h>
        #include <unistd.h>
        #include <atomic>

        #include "ricxfcpp/xapp.hpp"

        /*
            Information that the callback needs outside
            of what is given to it via parms on a call
            by the framework.
        */
        typedef struct {
            int        vlevel;             // verbosity level
            bool    forward;            // if true, message is forwarded
            int        stats_freq;         // header/stats after n messages
            std::atomic<long>    pcount; // messages processed
            std::atomic<long>    icount; // messages ignored
            std::atomic<int>    hdr;    // number of messages before next header
        } cb_info_t;

        // ----------------------------------------------------------------------

        /*
            Dump bytes to tty.
        */
        void dump( unsigned const char* buf, int len ) {
            int        i;
            int        j;
            char    cheater[17];

            fprintf( stdout, "<RD> 0000 | " );
            j = 0;
            for( i = 0; i < len; i++ ) {
                cheater[j++] =  isprint( buf[i] ) ? buf[i] : '.';
                fprintf( stdout, "%02x ", buf[i] );

                if( j == 16 ) {
                    cheater[j] = 0;
                    fprintf( stdout, " | %s\\n<RD> %04x | ", cheater, i+1 );
                    j = 0;
                }
            }

            if( j ) {
                i = 16 - (i % 16);
                for( ; i > 0; i-- ) {
                    fprintf( stdout, "   " );
                }
                cheater[j] = 0;
                fprintf( stdout, " | %s\\n", cheater );
            }
        }

        /*
            generate stats when the hdr count reaches 0. Only one active
            thread will ever see it be exactly 0, so this is thread safe.
        */
        void stats( cb_info_t& cbi ) {
            int curv;                    // current stat trigger value

            curv = cbi.hdr--;

            if( curv == 0 ) {                    // stats when we reach 0
                fprintf( stdout, "ignored: %ld  processed: %ld\\n",
                    cbi.icount.load(), cbi.pcount.load() );
                if( cbi.vlevel > 0 ) {
                    fprintf( stdout, "\\n     %5s %5s %2s %5s\\n",
                        "MTYPE", "SUBID", "ST", "PLLEN" );
                }

                cbi.hdr = cbi.stats_freq;        // reset must be last
            }
        }

        void cb1( Message& mbuf, int mtype, int subid, int len,
                        Msg_component payload,  void* data ) {
            cb_info_t*    cbi;
            long total_count;

            if( (cbi = (cb_info_t *) data) == NULL ) {
                return;
            }

            cbi->pcount++;
            stats( *cbi );            // gen stats & header if needed

            if( cbi->vlevel > 0 ) {
                fprintf( stdout, "<RD> %-5d %-5d %02d %-5d \\n",
                        mtype, subid, mbuf.Get_state(), len );

                if( cbi->vlevel > 1 ) {
                    dump(  payload.get(), len > 64 ? 64 : len );
                }
            }

            if( cbi->forward ) {
                // forward with no change to len or payload
                mbuf.Send_msg( Message::NO_CHANGE, NULL );
            }
        }

        /*
            registered as the default callback; it counts the
            messages that we aren't giving details about.
        */
        void cbd( Message& mbuf, int mtype, int subid, int len,
                        Msg_component payload,  void* data ) {
            cb_info_t*    cbi;

            if( (cbi = (cb_info_t *) data) == NULL ) {
                return;
            }

            cbi->icount++;
            stats( *cbi );

            if( cbi->forward ) {
                // forward with no change to len or payload
                mbuf.Send_msg( Message::NO_CHANGE, NULL );
            }
        }

        int main( int argc, char** argv ) {
            std::unique_ptr<Xapp> x;
            char*    port = (char *) "4560";
            int ai = 1;                    // arg processing index
            cb_info_t*    cbi;
            int        ncb = 0;            // number of callbacks registered
            int        mtype;
            int        nthreads = 1;

            cbi = (cb_info_t *) malloc( sizeof( *cbi ) );
            cbi->pcount = 0;
            cbi->icount = 0;
            cbi->stats_freq = 10;

            ai = 1;
            // very simple flag parsing (no error/bounds checking)
            while( ai < argc ) {
                if( argv[ai][0] != '-' )  {        // break on first non-flag
                    break;
                }

                // very simple arg parsing; each must be separate -x -y not -xy.
                switch( argv[ai][1] ) {
                    case 'f':                    // enable packet forwarding
                        cbi->forward = true;
                        break;

                    case 'p':                     // define port
                        port = argv[ai+1];
                        ai++;
                        break;

                    case 's':                        // stats frequency
                        cbi->stats_freq = atoi( argv[ai+1] );
                        if( cbi->stats_freq < 5 ) {    // enforce sanity
                            cbi->stats_freq = 5;
                        }
                        ai++;
                        break;

                    case 't':                        // thread count
                        nthreads = atoi( argv[ai+1] );
                        if( nthreads < 1 ) {
                            nthreads = 1;
                        }
                        ai++;
                        break;

                    case 'v':            // simple verbose bump
                        cbi->vlevel++;
                        break;

                    case 'V':            // explicit verbose level
                        cbi->vlevel = atoi( argv[ai+1] );
                        ai++;
                        break;

                    default:
                        fprintf( stderr, "unrecognised option: %s\\n", argv[ai] );
                        fprintf( stderr, "usage: %s [-f] [-p port] "
                                        "[-s stats-freq]  [-t thread-count] "
                                        "[-v | -V n] msg-type1 ... msg-typen\\n",
                                        argv[0] );
                        fprintf( stderr, "\\tstats frequency is based on # of messages received\\n" );
                        fprintf( stderr, "\\tverbose levels (-V) 0 counts only, "
                                        "1 message info 2 payload dump\\n" );
                        exit( 1 );
                }

                ai++;
            }

            cbi->hdr = cbi->stats_freq;
            fprintf( stderr, "<RD> listening on port: %s\\n", port );

            // create xapp, wait for route table if forwarding
            x = std::unique_ptr<Xapp>( new Xapp( port, cbi->forward ) );

            // register callback for each type on the command line
            while( ai < argc ) {
                mtype = atoi( argv[ai] );
                ai++;
                fprintf( stderr, "<RD> capturing messages for type %d\\n", mtype );
                x->Add_msg_cb( mtype, cb1, cbi );
                ncb++;
            }

            if( ncb < 1 ) {
                fprintf( stderr, "<RD> no message types specified on the command line\\n" );
                exit( 1 );
            }

            x->Add_msg_cb( x->DEFAULT_CALLBACK, cbd, cbi );        // register default cb

            fprintf( stderr, "<RD> starting driver\\n" );
            x->Run( nthreads );

            // return from run() is not expected, but some compilers might
            // compilain if there isn't a return value here.
            return 0;
        }

      Figure 8: Simple callback application.


Callback Receiver
-----------------

      This sample programme implements a simple message listener
      which registers three callback functions to process two
      specific message types and a default callback to handle
      unrecognised messages.

      When a message of type 1 is received, it will send two
      response messages back to the sender. Two messages are sent
      in order to illustrate that it is possible to send multiple
      responses using the same received message.

      The programme illustrates how multiple listening threads can
      be used, but the programme is **not** thread safe; to keep
      this example as simple as possible, the counters are not
      locked when incremented.


      ::
        #include <stdio.h>

        #include "ricxfcpp/message.hpp"
        #include "ricxfcpp/msg_component.hpp"
        #include "ricxfcpp/xapp.hpp"

        // counts; not thread safe
        long cb1_count = 0;
        long cb2_count = 0;
        long cbd_count = 0;

        long cb1_lastts = 0;
        long cb1_lastc = 0;

        // respond with 2 messages for each type 1 received
        void cb1( Message& mbuf, int mtype, int subid, int len,
                    Msg_component payload,  void* data ) {
            long now;
            long total_count;

            // illustrate that we can use the same buffer for 2 rts calls
            mbuf.Send_response( 101, -1, 5, (unsigned char *) "OK1\\n" );
            mbuf.Send_response( 101, -1, 5, (unsigned char *) "OK2\\n" );

            cb1_count++;
        }

        // just count messages
        void cb2( Message& mbuf, int mtype, int subid, int len,
                    Msg_component payload,  void* data ) {
            cb2_count++;
        }

        // default to count all unrecognised messages
        void cbd( Message& mbuf, int mtype, int subid, int len,
                    Msg_component payload,  void* data ) {
            cbd_count++;
        }

        int main( int argc, char** argv ) {
            Xapp* x;
            char*    port = (char *) "4560";
            int ai = 1;                            // arg processing index
            int nthreads = 1;

            // very simple flag processing (no bounds/error checking)
            while( ai < argc ) {
                if( argv[ai][0] != '-' )  {
                    break;
                }

                switch( argv[ai][1] ) {            // we only support -x so -xy must be -x -y
                    case 'p':
                        port = argv[ai+1];
                        ai++;
                        break;

                    case 't':
                        nthreads = atoi( argv[ai+1] );
                        ai++;
                        break;
                }

                ai++;
            }

            fprintf( stderr, "<XAPP> listening on port: %s\\n", port );
            fprintf( stderr, "<XAPP> starting %d threads\\n", nthreads );

            x = new Xapp( port, true );
            x->Add_msg_cb( 1, cb1, NULL );                // register callbacks
            x->Add_msg_cb( 2, cb2, NULL );
            x->Add_msg_cb( x->DEFAULT_CALLBACK, cbd, NULL );

            x->Run( nthreads );                // let framework drive
            // control should not return
        }

      Figure 9: Simple callback application.



Looping Sender
--------------

      This is another very simple application which demonstrates
      how an application can control its own listen loop while
      sending messages. As with the other examples, some error
      checking is skipped, and short cuts have been made in order
      to keep the example small and to the point.


      ::

        #include <stdio.h>
        #include <string.h>
        #include <unistd.h>

        #include <iostream>
        #include <memory>

        #include "ricxfcpp/xapp.hpp"

        extern int main( int argc, char** argv ) {
            std::unique_ptr<Xapp> xfw;
            std::unique_ptr<Message> msg;
            Msg_component payload;                // special type of unique pointer to the payload

            int    sz;
            int len;
            int i;
            int ai;
            int response_to = 0;                // max timeout wating for a response
            char*    port = (char *) "4555";
            int    mtype = 0;
            int rmtype;                            // received message type
            int delay = 1000000;                // mu-sec delay; default 1s


            // very simple flag processing (no bounds/error checking)
            while( ai < argc ) {
                if( argv[ai][0] != '-' )  {
                    break;
                }

                // we only support -x so -xy must be -x -y
                switch( argv[ai][1] ) {
                    // delay between messages (mu-sec)
                    case 'd':
                        delay = atoi( argv[ai+1] );
                        ai++;
                        break;

                    case 'p':
                        port = argv[ai+1];
                        ai++;
                        break;

                    // timeout in seconds; we need to convert to ms for rmr calls
                    case 't':
                        response_to = atoi( argv[ai+1] ) * 1000;
                        ai++;
                        break;
                }
                ai++;
            }

            fprintf( stderr, "<XAPP> response timeout set to: %d\\n", response_to );
            fprintf( stderr, "<XAPP> listening on port: %s\\n", port );

            // get an instance and wait for a route table to be loaded
            xfw = std::unique_ptr<Xapp>( new Xapp( port, true ) );
            msg = xfw->Alloc_msg( 2048 );

            for( i = 0; i < 100; i++ ) {
                mtype++;
                if( mtype > 10 ) {
                    mtype = 0;
                }

                // we'll reuse a received message; get max size
                sz = msg->Get_available_size();

                // direct access to payload; add something silly
                payload = msg->Get_payload();
                len = snprintf( (char *) payload.get(), sz, "This is message %d\\n", i );

                // payload updated in place, prevent copy by passing nil
                if ( ! msg->Send_msg( mtype, Message::NO_SUBID,  len, NULL )) {
                    fprintf( stderr, "<SNDR> send failed: %d\\n", i );
                }

                // receive anything that might come back
                msg = xfw->Receive( response_to );
                if( msg != NULL ) {
                    rmtype = msg->Get_mtype();
                    payload = msg->Get_payload();
                    fprintf( stderr, "got: mtype=%d payload=(%s)\\n",
                        rmtype, (char *) payload.get() );
                } else {
                    msg = xfw->Alloc_msg( 2048 );
                }

                if( delay > 0 ) {
                    usleep( delay );
                }
            }
        }

      Figure 10: Simple looping sender application.

