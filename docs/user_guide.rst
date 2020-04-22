     
 
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
 
Introduction 
============================================================================================ 
 
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
 
The Framework API 
============================================================================================ 
 
The C++ framework API consists of the creation of the xApp 
object, and invoking desired functions via the instance of 
the object. The following paragraphs cover the various steps 
involved to create an xApp instance, wait for a route table 
to arrive, send a message, and wait for messages to arrive. 
 
Creating the xApp instance 
-------------------------------------------------------------------------------------------- 
 
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
       
      Listening For Messages 
      ============================================================================================ 
       
      The program in the previous example can be extended with just 
      a few lines of code to enable it to receive and process 
      messages. The application needs to register a callback 
      function for each message type which it desires to process. 
       
      Once registered, each time a message is received the 
      registered callback for the message type will be invoked by 
      the framework. 
       
      Callback Signature 
      -------------------------------------------------------------------------------------------- 
       
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
      -------------------------------------------------------------------------------------------- 
       
      The xApp may also register a default callback function such 
      that the function will be invoked for any message that does 
      not have a registered callback. If the xAPP does not register 
      a default callback, any message which cannot be mapped to a 
      known callback function is silently dropped. A default 
      callback is registered by providing a *generic* message type 
      of xapp->DEFAULT_CALLBACK on an Add_msg_cb call. 
       
      The Framework Callback Driver 
      -------------------------------------------------------------------------------------------- 
       
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
       
      Sending Messages 
      ============================================================================================ 
       
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
      -------------------------------------------------------------------------------------------- 
       
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
      -------------------------------------------------------------------------------------------- 
       
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
      -------------------------------------------------------------------------------------------- 
       
      Not all xApps will be "responders," meaning that some xApps 
      will need to send one or more messages before they can expect 
      to receive any messages back. To accomplish this, the xApp 
      must first allocate a message buffer, optionally initialising 
      the payload, and then using the message's Send_msg() function 
      to send a message out. The framework's Alloc_msg() function 
      can be used to create a Message object with a desired payload 
      size. 
       
      Framework Provided Callbacks 
      ============================================================================================ 
       
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
       
      Example Programmes 
      ============================================================================================ 
       
      The following sections contain several example programmes 
      which are written on top of the C++ framework. 
       
      RMR Dump xAPP 
      -------------------------------------------------------------------------------------------- 
       
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
         
