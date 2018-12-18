// kernel.cc 
//	Initialization and cleanup routines for the Nachos kernel.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "main.h"
#include "kernel.h"
#include "sysdep.h"
#include "synch.h"
#include "synchlist.h"
#include "libtest.h"
#include "string.h"
#include "synchconsole.h"
#include "synchdisk.h"
#include "post.h"
#include "map"
#include "list.h"


//----------------------------------------------------------------------
// Kernel::Kernel
// 	Interpret command line arguments in order to determine flags 
//	for the initialization (see also comments in main.cc)  
//----------------------------------------------------------------------
int Kernel::swapCounter=0;
Kernel::Kernel(int argc, char **argv)
{
    randomSlice = FALSE; 
    debugUserProg = FALSE;
    consoleIn = NULL;          // default is stdin
    consoleOut = NULL;         // default is stdout
#ifndef FILESYS_STUB
    formatFlag = FALSE;
#endif
    reliability = 1;            // network reliability, default is 1.0
    hostName = 0;               // machine id, also UNIX socket name
                                // 0 is the default machine id
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-rs") == 0) {
 	    ASSERT(i + 1 < argc);
	    RandomInit(atoi(argv[i + 1]));// initialize pseudo-random
					// number generator
	    randomSlice = TRUE;
	    i++;
        } else if (strcmp(argv[i], "-s") == 0) {
            debugUserProg = TRUE;
	} else if (strcmp(argv[i], "-ci") == 0) {
	    ASSERT(i + 1 < argc);
	    consoleIn = argv[i + 1];
	    i++;
	} else if (strcmp(argv[i], "-co") == 0) {
	    ASSERT(i + 1 < argc);
	    consoleOut = argv[i + 1];
	    i++;
#ifndef FILESYS_STUB
	} else if (strcmp(argv[i], "-f") == 0) {
	    formatFlag = TRUE;
#endif
        } else if (strcmp(argv[i], "-n") == 0) {
            ASSERT(i + 1 < argc);   // next argument is float
            reliability = atof(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-m") == 0) {
            ASSERT(i + 1 < argc);   // next argument is int
            hostName = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-u") == 0) {
            cout << "Partial usage: nachos [-rs randomSeed]\n";
	    cout << "Partial usage: nachos [-s]\n";
            cout << "Partial usage: nachos [-ci consoleIn] [-co consoleOut]\n";
#ifndef FILESYS_STUB
	    cout << "Partial usage: nachos [-nf]\n";
#endif
            cout << "Partial usage: nachos [-n #] [-m #]\n";
	}
    else if(strcmp(argv[i], "-quantum") == 0){
        // printf("quantum%d\n", quantum);
        quantum =atoi(argv[i+1]);
        //printf("quantum%d\n", quantum);
    }
    }
}

//----------------------------------------------------------------------
// Kernel::Initialize
// 	Initialize Nachos global data structures.  Separate from the 
//	constructor because some of these refer to earlier initialized
//	data via the "kernel" global variable.
//----------------------------------------------------------------------
static void NetworkServer(int which){
    printf("\n=======kernel started=========\n\n");
    while(true){
     if(!kernel->processMap.empty()){
        // cout<<"processmap not empty in one"<<endl;
         if(!kernel->waitMessageThreads->IsEmpty()){ 
         // printf("wait messge not empty in one\n");       
        ListIterator<Thread*> *it = new ListIterator<Thread*>(kernel->waitMessageThreads);
         for (; !it->IsDone(); it->Next()) {
            if(!it->Item()->bufferList->IsEmpty()){
                // printf("buffer not empty in onw\n");
            ListIterator<Buffer*> *itb = new ListIterator<Buffer*>(it->Item()->bufferList);
                for (; !itb->IsDone(); itb->Next()) {
                    //string threadName(it->Item()->getName());
                    if(kernel->processMap[itb->Item()->getSender()]!=NULL){
                    if(itb->Item()->getMessages()!=""){
                     IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
                    if(!kernel->scheduler->IsInReadyList(it->Item())){
                    cout<<"Inside kernel: Message received for the process "<<itb->Item()->getReceiver()<<" from "<<itb->Item()->getSender()<<"In the buffer id"<<itb->Item()->getBufferId()<<endl;
                    cout<<"Inside kernel: Putting receiver thread of the same in ReadyToRun state"<<endl;
                    kernel->scheduler->ReadyToRun(it->Item());
                    kernel->waitMessageThreads->Remove(it->Item());
                    }
                    (void) kernel->interrupt->SetLevel(oldLevel);
                    }
                }
            }
                }
                else{
                    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
                    if(!kernel->scheduler->IsInReadyList(it->Item())){
                    kernel->waitMessageThreads->Remove(it->Item());
                    //cout<<"Inside kernel: Message received for the process "<<itb->Item()->getReceiver()<<" from "<<itb->Item()->getSender()<<"In the buffer id"<<itb->Item()->getBufferId()<<endl;
                    cout<<"Inside kernel: The process "<<it->Item()->getName()<<" waiting for a message without having a communication buffer"<<endl;
                    cout<<"Inside kernel: No other process available in the kernel scheduler to run"<<endl;
                    cout<<"Inside kernel: Putting receiver thread in ReadyToRun state"<<endl;
                    kernel->scheduler->ReadyToRun(it->Item());
                    }
                    (void) kernel->interrupt->SetLevel(oldLevel);
                    }   
                }
                }
                
            
         
        if(!kernel->waitAnswerThreads->IsEmpty()){ 

        ListIterator<Thread*> *ita = new ListIterator<Thread*>(kernel->waitAnswerThreads);
         for (; !ita->IsDone(); ita->Next()) {
                // cout<<"wt:"<<ita->Item()->getName();
            ListIterator<Buffer*> *itba = new ListIterator<Buffer*>(ita->Item()->bufferList);
                for (; !itba->IsDone(); itba->Next()) {
                    // printf("in buff iter one\n");
                    if(kernel->processMap[itba->Item()->getSender()]!=NULL){
                        // printf("persent in process map one\n");
                    if(itba->Item()->getAnswer()!=""){
                    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
                    if(!kernel->scheduler->IsInReadyList(ita->Item())){
                    cout<<"Inside kernel: Answer received for the process "<<itba->Item()->getReceiver()<<" from "<<itba->Item()->getSender()<<"in the buffer id :"<<itba->Item()->getBufferId()<<endl;
                    // printf("ANSWER: %s\n", itba->Item()->getAnswer().c_str());
                    cout<<"Inside kernel: Putting receiver thread of the same in ReadyToRun state"<<endl;
                    kernel->scheduler->ReadyToRun(ita->Item());
                    kernel->waitAnswerThreads->Remove(ita->Item());                 
                    }
                    (void) kernel->interrupt->SetLevel(oldLevel);
                    }
                }
                else{
                    // printf("not in process map ans one\n");
                    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
                    if(!kernel->scheduler->IsInReadyList(ita->Item())){
                    // cout<<"Inside kernel: Answer received for the process "<<itba->Item()->getReceiver()<<" from "<<itba->Item()->getSender()<<"in the buffer id"<<itba->Item()->getBufferId()<<endl;
                    cout<<"Inside kernel: Putting receiver thread in ReadyToRun state"<<endl;
                    kernel->scheduler->ReadyToRun(ita->Item());
                    kernel->waitAnswerThreads->Remove(ita->Item());  
                    }
                    (void) kernel->interrupt->SetLevel(oldLevel);
                }
                }
                }
            }
     }
     if(kernel->scheduler->IsReadyListEmpty())
     {
            cout<<"Inside kernnal: No more user programs to execute!"<<endl;
            cout<<"Inside kernel: checking for any receiver process which are waiting for message."<<endl;
            if(!kernel->processMap.empty()){
                bool check=FALSE;
                //start
        if(!kernel->waitMessageThreads->IsEmpty()){        
        ListIterator<Thread*> *it = new ListIterator<Thread*>(kernel->waitMessageThreads);
         for (; !it->IsDone(); it->Next()) {
            if(!it->Item()->bufferList->IsEmpty()){
            ListIterator<Buffer*> *itb = new ListIterator<Buffer*>(it->Item()->bufferList);
                for (; !itb->IsDone(); itb->Next()) {
                    //if(itb->Item()->getMessages()!=""){
                     IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
                    if(!kernel->scheduler->IsInReadyList(it->Item())){
                    check=TRUE;
                    if(itb->Item()->getMessages()!=""){
                        cout<<"Inside kernel: Message received for the process "<<itb->Item()->getReceiver()<<" from "<<itb->Item()->getSender()<<"In the buffer id"<<itb->Item()->getBufferId()<<endl;                   
                    }
                    else{
                        cout<<"Inside kernel: No message received for the process "<<itb->Item()->getReceiver()<<" from "<<itb->Item()->getSender()<<"In the buffer id"<<itb->Item()->getBufferId()<<endl;
                    }
                    //cout<<"Inside kernel: Message received for the process "<<itb->Item()->getReceiver()<<" from "<<itb->Item()->getSender()<<"In the buffer id"<<itb->Item()->getBufferId()<<endl;
                    cout<<"Inside kernel: Putting receiver thread of the "<<itb->Item()->getReceiver()<<" in ReadyToRun state"<<endl;
                    kernel->scheduler->ReadyToRun(it->Item());
                    kernel->waitMessageThreads->Remove(it->Item());
                    }
                    (void) kernel->interrupt->SetLevel(oldLevel);
                    
                }
                }
                else{
                     IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
                    if(!kernel->scheduler->IsInReadyList(it->Item())){
                    kernel->scheduler->ReadyToRun(it->Item());
                    kernel->waitMessageThreads->Remove(it->Item());
                    }
                    (void) kernel->interrupt->SetLevel(oldLevel);
                }
            }
         }
        if(!kernel->waitAnswerThreads->IsEmpty()){ 
            // printf("wait ans not empty 2\n");
        ListIterator<Thread*> *ita = new ListIterator<Thread*>(kernel->waitAnswerThreads);
         for (; !ita->IsDone(); ita->Next()) {
            // cout<<"WT:"<<ita->Item()->getName();
            if(!ita->Item()->bufferList->IsEmpty()){
            ListIterator<Buffer*> *itba = new ListIterator<Buffer*>(ita->Item()->bufferList);
                for (; !itba->IsDone(); itba->Next()) {
                    // printf("in buff iter 2\n");
                    //if(itba->Item()->getAnswer()!=""){
                     IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
                    if(!kernel->scheduler->IsInReadyList(ita->Item())){
                        // printf("not in ready list ans 2\n");
                    check=TRUE;
                    if(itba->Item()->getAnswer()!=""){    
                    cout<<"Inside kernel: Answer received for the process "<<itba->Item()->getReceiver()<<" from "<<itba->Item()->getSender()<<"in the buffer id "<<itba->Item()->getBufferId()<<endl;
                    cout<<itba->Item()->getAnswer()<<endl;
                    }
                    else{
                    cout<<"No answer received for the process "<<itba->Item()->getReceiver()<<" from "<<itba->Item()->getSender()<<"In the buffer id"<<itba->Item()->getBufferId()<<endl;
                    }
                    cout<<"Inside kernel: Putting receiver thread of the same in ReadyToRun state"<<endl;
                    kernel->scheduler->ReadyToRun(ita->Item());
                    kernel->waitAnswerThreads->Remove(ita->Item());   
                    }
                    else{
                        // printf("in ready list ans 2\n");
                    }
                    (void) kernel->interrupt->SetLevel(oldLevel);
                    //}
                }
                }
                else{
                     IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
                    if(!kernel->scheduler->IsInReadyList(ita->Item())){
                    kernel->scheduler->ReadyToRun(ita->Item());
                    kernel->waitMessageThreads->Remove(ita->Item());
                    }
                    (void) kernel->interrupt->SetLevel(oldLevel);
                }

                }
            }
            //end
            if(!check){
                cout<<"Inside kernel: No receiver processes are waiting for message."<<endl;
                cout<<"Finishing kernel thread"<<endl;
                //cout<<"Kernel thread Finished execution"<<endl;
                cout<<"\n******************END OF EXECUTION****************"<<endl;
                kernel->currentThread->Finish();
            }else{
            cout<<"Inside kernel: Yielding server thread to execute programs which didnt receive any messages"<<endl;
            kernel->currentThread->Yield();
            }

            }
            else{
            cout<<"Inside kernel: No receiver processes are waiting for message or answer."<<endl;
            cout<<"Finishing kernel thread"<<endl;
            //cout<<"Kernel thread Finished execution"<<endl;
            cout<<"\n******************END OF EXECUTION****************"<<endl;
            kernel->currentThread->Finish();
            }

        }
        else{
            cout<<"Inside kernel: Yielding server thread to execute other programs"<<endl;
            kernel->currentThread->Yield();
        }
    }
}


// static void NetworkServer(int which){
//     printf("\n=======Network server started=========\n\n");
//     while(true){
//         //printf("Network server started\n");
//         if(!kernel->kernelPorts.empty()){
//         for(map <int, Port*>:: iterator iter= kernel->kernelPorts.begin(); iter !=kernel->kernelPorts.end(); iter++) {
//         //printf("in loop\n");
//             if(iter->second!=NULL){
//                 // printf("port not null %d\n",iter->first);
//                 Thread* thread=iter->second->getReceiverThread();
//                 if(thread!=NULL){
//                     if(iter->second->getMessagesReceived()!=""){
//                     IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
//                     if(!kernel->scheduler->IsInReadyList(thread)){
//                     printf("Inside network server: Message received for the process with port number %d\n",iter->first);
//                     cout<<"Inside network server: Putting receiver thread of the same in ReadyToRun state"<<endl;
//                     kernel->scheduler->ReadyToRun(thread);
//                     }
//                     (void) kernel->interrupt->SetLevel(oldLevel);
//                     }
//                 }                    
//             }
//         }
//         }
//         if(kernel->scheduler->IsReadyListEmpty()){
//             cout<<"Inside network server: No more user programs to execute!"<<endl;
//             cout<<"Inside network server: checking for any receiver process which are waiting for message."<<endl;
//             if(!kernel->kernelPorts.empty()){
//                 bool check=FALSE;
//             for(map <int, Port*>:: iterator iter1= kernel->kernelPorts.begin(); iter1 !=kernel->kernelPorts.end(); iter1++) {
//         //printf("in loop\n");
//             if(iter1->second!=NULL){
//                 // printf("port not null %d\n",iter->first);
//                 Thread* thread1=iter1->second->getReceiverThread();
//                 if(thread1!=NULL){
//                     check=TRUE;
//                     IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
//                     if(!kernel->scheduler->IsInReadyList(thread1)){
//                     cout<<"Inside network server: Putting receiver thread of the process with port number "<<iter1->first<<" in ReadyToRun state"<<endl;
//                     kernel->scheduler->ReadyToRun(thread1);
//                     }
//                     (void) kernel->interrupt->SetLevel(oldLevel);
//                     }
//                     else{
//                         if(iter1->second->getMessagesReceived()!=""){
//                         cout<<"Message for process with port number "<<iter1->first<<" cannot be delivered as there is no receiver process"<<endl;
//                         kernel->kernelPorts.erase(iter1->first);
//                         cout<<"Message removed from kernel buffer"<<endl;
//                     }
//                     }
//                 }
//             }
//             if(!check){
//                 cout<<"Inside network server: No receiver processes are waiting for message."<<endl;
//                 cout<<"Finishing network server thread"<<endl;
//                 kernel->currentThread->Finish();
//             }else{
//             cout<<"Inside network server: Yielding server thread to execute programs which didnt receive any messages"<<endl;
//             kernel->currentThread->Yield();
//             }
//             }
//             else{
//             cout<<"Inside network server: No receiver processes are waiting for message."<<endl;
//             cout<<"Finishing network server thread"<<endl;
//             kernel->currentThread->Finish();
//             }
//         }
//         else{
//             cout<<"Inside network server: Yielding server thread to execute other programs"<<endl;
//             kernel->currentThread->Yield();    
//         }
        
//     }
// }

void
Kernel::Initialize()
{
    // We didn't explicitly allocate the current thread we are running in.
    // But if it ever tries to give up the CPU, we better have a Thread
    // object to save its state. 
    currentThread = new Thread("main");		
    currentThread->setStatus(RUNNING);
    phyPageNos = new List<int>();
    bmap= new Bitmap(NumPhysPages);
    bufferBitmap =new Bitmap(10);
    waitMessageThreads= new List<Thread*>();
    waitAnswerThreads= new List<Thread*>();
    stats = new Statistics();		// collect statistics
    interrupt = new Interrupt;		// start up interrupt handling
    scheduler = new Scheduler();	// initialize the ready queue
    alarm = new Alarm(randomSlice,quantum);	// start up time slicing
    machine = new Machine(debugUserProg);
    synchConsoleIn = new SynchConsoleInput(consoleIn); // input from stdin
    synchConsoleOut = new SynchConsoleOutput(consoleOut); // output to stdout
    synchDisk = new SynchDisk();    //
#ifdef FILESYS_STUB
    fileSystem = new FileSystem();
#else
    fileSystem = new FileSystem(formatFlag);
#endif // FILESYS_STUB
    postOfficeIn = new PostOfficeInput(10);
    postOfficeOut = new PostOfficeOutput(reliability);
    bool issuccess=kernel->fileSystem->Create("Swapfile");
    if(issuccess){
        swapfileptr = kernel->fileSystem->Open("Swapfile");
        // cout<<"\n Swap file created successfully"<<endl;
    }
    else{
        // cout<<"\nSwap file creation failed"<<endl;
    }
    //assignment 3 changes
    Thread* serverThread = new Thread("kernel thread");
    serverThread->Fork((VoidFunctionPtr) NetworkServer, (void*) 1);
    //kernel->currentThread->Yield();
    interrupt->Enable();
}

int
Kernel::getSwapLocation(){
    return swapCounter++;
}

OpenFile*
Kernel::getSwapFile(){
    return swapfileptr;
}
//----------------------------------------------------------------------
// Kernel::~Kernel
// 	Nachos is halting.  De-allocate global data structures.
//----------------------------------------------------------------------

Kernel::~Kernel()
{
    delete stats;
    delete interrupt;
    delete scheduler;
    delete alarm;
    delete machine;
    delete synchConsoleIn;
    delete synchConsoleOut;
    delete synchDisk;
    delete fileSystem;
    delete postOfficeIn;
    delete postOfficeOut;
    
    Exit(0);
}

//----------------------------------------------------------------------
// Kernel::ThreadSelfTest
//      Test threads, semaphores, synchlists
//----------------------------------------------------------------------

void
Kernel::ThreadSelfTest() {
   Semaphore *semaphore;
   SynchList<int> *synchList;
   
   LibSelfTest();		// test library routines
   currentThread->SelfTest();	// test thread switching
   
   				// test semaphore operation
   semaphore = new Semaphore("test", 0);
   semaphore->SelfTest();
   delete semaphore;
   
   				// test locks, condition variables
				// using synchronized lists
   synchList = new SynchList<int>;
   synchList->SelfTest(9);
   delete synchList;

}

//----------------------------------------------------------------------
// Kernel::ConsoleTest
//      Test the synchconsole
//----------------------------------------------------------------------

void
Kernel::ConsoleTest() {
    char ch;

    cout << "Testing the console device.\n" 
        << "Typed characters will be echoed, until ^D is typed.\n"
        << "Note newlines are needed to flush input through UNIX.\n";
    cout.flush();

    do {
        ch = synchConsoleIn->GetChar();
        if(ch != EOF) synchConsoleOut->PutChar(ch);   // echo it!
    } while (ch != EOF);

    cout << "\n";

}

//----------------------------------------------------------------------
// Kernel::NetworkTest
//      Test whether the post office is working. On machines #0 and #1, do:
//
//      1. send a message to the other machine at mail box #0
//      2. wait for the other machine's message to arrive (in our mailbox #0)
//      3. send an acknowledgment for the other machine's message
//      4. wait for an acknowledgement from the other machine to our 
//          original message
//
//  This test works best if each Nachos machine has its own window
//----------------------------------------------------------------------

void
Kernel::NetworkTest() {

    if (hostName == 0 || hostName == 1) {
        // if we're machine 1, send to 0 and vice versa
        int farHost = (hostName == 0 ? 1 : 0); 
        PacketHeader outPktHdr, inPktHdr;
        MailHeader outMailHdr, inMailHdr;
        char *data = "Hello there!";
        char *ack = "Got it!";
        char buffer[MaxMailSize];

        // construct packet, mail header for original message
        // To: destination machine, mailbox 0
        // From: our machine, reply to: mailbox 1
        outPktHdr.to = farHost;         
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.length = strlen(data) + 1;

        // Send the first message
        postOfficeOut->Send(outPktHdr, outMailHdr, data); 

        // Wait for the first message from the other machine
        postOfficeIn->Receive(0, &inPktHdr, &inMailHdr, buffer);
        cout << "Got: " << buffer << " : from " << inPktHdr.from << ", box " 
                                                << inMailHdr.from << "\n";
        cout.flush();

        // Send acknowledgement to the other machine (using "reply to" mailbox
        // in the message that just arrived
        outPktHdr.to = inPktHdr.from;
        outMailHdr.to = inMailHdr.from;
        outMailHdr.length = strlen(ack) + 1;
        postOfficeOut->Send(outPktHdr, outMailHdr, ack); 

        // Wait for the ack from the other machine to the first message we sent
	postOfficeIn->Receive(1, &inPktHdr, &inMailHdr, buffer);
        cout << "Got: " << buffer << " : from " << inPktHdr.from << ", box " 
                                                << inMailHdr.from << "\n";
        cout.flush();
    }

    // Then we're done!
}

