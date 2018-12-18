// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include <map>
#include "thread.h"
#include <string>
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------
map< int, Thread* > threadMap; 
void Exit_POS(int id){
	cout<<"\n*****Exit_POS Handler*****"<<endl;
	cout<<"----------------------------------\n"<<endl;
	Interrupt *interrupt = kernel->interrupt;
	Scheduler *scheduler = kernel->scheduler;
	IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);;
	Thread* pThread= threadMap[id];
	if(pThread!=NULL){
		cout<<"*Inserting parent on ready list of scheduler*"<<endl;
		kernel->scheduler->ReadyToRun(pThread);
	}
	(void) interrupt->SetLevel(oldLevel);
	}
void ForkTest1(int id)
{
	printf("ForkTest1 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest1 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	Exit_POS(id);
}

void ForkTest2(int id)
{
	printf("ForkTest2 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest2 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	Exit_POS(id);
}

void ForkTest3(int id)
{
	printf("ForkTest3 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest3 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	Exit_POS(id);
}

static void
copyArrays(char *buffer,char *mainMemory,int startindex){
	int size= strlen(buffer);
	for(int i=0;i<size;i++){
		mainMemory[i+startindex]=buffer[i];
	}
}
static void
copyToBuffer(char *buffer,char *mainMemory,int startindex){
	//int size= strlen(buffer);
	for(int i=0;i<PageSize;i++){
		buffer[i]=mainMemory[i+startindex];
	}
}
void
PageFaultHandler(int badVaddr){
	//int vpn= badVaddr/
	// Interrupt *interrupt = kernel->interrupt;
 //    Scheduler *scheduler = kernel->scheduler;
 //    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	cout<<"\n------Page fault exeption handler------"<<endl;
	int vpn=badVaddr/PageSize;
	int phyPageNo=kernel->bmap->FindAndSet();
	if(phyPageNo!=-1){
		char* buffer = new char[PageSize];
		//bzero(&kernel->machine->mainMemory[phyPageNo*PageSize], PageSize);
		cout<<"Free page available in main memory"<<endl;
		TranslationEntry *pageTableEntry = kernel->currentThread->space->getEntry(vpn);
		//cout<<"before swap file"<<endl;
		//cout<<pageTableEntry->SwapLocation*PageSize<<endl;
		cout<<"Loading page from swap file"<<endl;
		kernel->getSwapFile()->ReadAt(buffer,PageSize,pageTableEntry->SwapLocation*PageSize);
		//cout<<"before array copy"<<endl;
		bzero(&kernel->machine->mainMemory[phyPageNo*PageSize],PageSize);
		memcpy(&kernel->machine->mainMemory[phyPageNo*PageSize],buffer,PageSize);
		cout<<"Inserting the loaded page to main memory in physical page number - "<<phyPageNo<<endl;
		//copyArrays(buffer,kernel->machine->mainMemory,phyPageNo*PageSize);
		//cout<<"array copied "<<endl;
		pageTableEntry->physicalPage=phyPageNo;
		pageTableEntry->valid = TRUE;
		pageTableEntry->use = TRUE;
		kernel->ppnToThreadMap[phyPageNo]=kernel->currentThread;
		kernel->phyPageNos->Append(phyPageNo);
		//cout<<"end of if"<<endl;
	}
	else{
		cout<<"No free space in main memory"<<endl;
		int evictpn=kernel->phyPageNos->RemoveFront();
		cout<<"Evicting "<<evictpn<<" page from main memory "<<endl;
		TranslationEntry *pageTableEntry = kernel->ppnToThreadMap[evictpn]->space->getEntryByPPN(evictpn);
		char* buffer = new char[PageSize];
		copyToBuffer(buffer,kernel->machine->mainMemory,evictpn*PageSize);
		cout<<"Inserting evicted page into swap file"<<endl;
		kernel->getSwapFile()->WriteAt(buffer,PageSize,pageTableEntry->SwapLocation*PageSize);
		pageTableEntry->physicalPage=-1;
		pageTableEntry->valid=FALSE;
		pageTableEntry->use=FALSE;
		kernel->bmap->Clear(evictpn);
		//PageFaultHandler(badVaddr);		
	}
	cout<<"------End of page fault handler------\n"<<endl;
	//(void) interrupt->SetLevel(oldLevel);
}

// void
// portAssignHandler(int port){
// 	cout<<"\n*****PortAssign Syscall Handler*****\n"<<endl;
// 	int success=0;
// 	if(kernel->kernelPorts[port]!=NULL){
// 	if(kernel->kernelPorts[port]->getReceiverThread() == NULL){
// 		kernel->kernelPorts[port]->setReceiverThread(kernel->currentThread);
// 		success=1;
// 		cout<<"Port "<<port<<" assigned to this process successfully"<<endl;
// 	}
// 	else{
// 		success=0;
// 		cout<<"Port "<<port<<" assigned to this process failed as this port number is already assigned to other process"<<endl;
// 	}
// 	}
// 	else{
// 		Port* p = new Port();
// 		p->setReceiverThread(kernel->currentThread);
// 		kernel->kernelPorts[port]=p;
// 		success=1;
// 		cout<<"Port "<<port<<" assigned to this process successfully"<<endl;
// 	}
// 	cout<<"\n*****end of PortAssign Syscall Handler*****\n";
// 	kernel->machine->WriteRegister(2, (int)success);
// 	/* Modify return point */
// 	{
// 		/* set previous programm counter (debugging only)*/
// 		kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

// 		/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
// 		kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

// 		/* set next programm counter for brach execution */
// 		kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
// 	}
// }

// int
// checkForBufferId(string sender, int receiver){
// string rec=convertIntToString(receiver);


// }

string convertIntToString(int loc){
	string str="";
	while(true){
		int output;
		bool isMem = kernel->machine->ReadMem(loc, 1, &output);
		if(isMem){
			if((char)output!=NULL){
			str.push_back((char)output);
			loc++;
			}
			else{
				break;
			}
		}
		else{
			break;
		}
	}
	return str;
}

void
SendMessageHandler(int receiverLoc,int messageLoc, int bufferId){
	//cout<<"\n*****SendMessage Syscall Handler*****\n"<<endl;
	string receiver=convertIntToString(receiverLoc);
	string message=convertIntToString(messageLoc);
	string str(kernel->currentThread->getName());
	// cout<<kernel->currentThread->getName();
	// cout<<str;
	Interrupt *interrupt = kernel->interrupt;
	Scheduler *scheduler = kernel->scheduler;
	IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	// while(true){
	// 	int output;
	// 	bool isMem = kernel->machine->ReadMem(receiverLoc, 1, &output);
	// 	if(isMem){
	// 		if((char)output!=NULL){
	// 		receiver.push_back((char)output);
	// 		receiverLoc++;
	// 		}
	// 		else{
	// 			break;
	// 		}
	// 	}
	// 	else{
	// 		break;
	// 	}
	// }
	// while(true){
	// 	int output;
	// 	bool isMem = kernel->machine->ReadMem(messageLoc, 1, &output);
	// 	if(isMem){
	// 		if((char)output!=NULL){
	// 		message.push_back((char)output);
	// 		messageLoc++;
	// 		}
	// 		else{
	// 			break;
	// 		}
	// 	}
	// 	else{
	// 		break;
	// 	}
	// }
	bool checkIfProcessExists =FALSE;
	for(map <string, Thread*>:: iterator iter= kernel->processMap.begin(); iter !=kernel->processMap.end(); iter++) {
        if(iter->first==receiver){
            checkIfProcessExists=TRUE;
        }
    }
	if(checkIfProcessExists){
		if(bufferId==-1){
		ListIterator<Buffer*> *it = new ListIterator<Buffer*>(kernel->processMap[receiver]->bufferList);
   		 bool check=FALSE;
   		 for (; !it->IsDone(); it->Next()) {
   		 	if(it->Item()->getSender()==str&&it->Item()->getReceiver()==receiver)
   		 	{
   		 		cout<<"Buffer already present for the communication between "<<str<<" and "<<receiver<<". Added the new message to the existing buffer "<<it->Item()->getBufferId()<<endl;
   		 		cout<<"Sent Message : "<<message<<endl;
   		 		check=TRUE;
   		 		it->Item()->addMessages(message);
   		 	}
   		 }
   		 if(!check){
   		 	int freebit=kernel->bufferBitmap->FindAndSet();
            if(freebit!=-1){
            	Buffer *buffer=new Buffer();
		buffer->setReceiver(receiver);
		buffer->setSender(str);
		buffer->setBufferId(freebit);
		buffer->addMessages(message);
		kernel->processMap[receiver]->bufferList->Append(buffer);
		cout<<"No exisitng buffer present. Created buffer for communication between "<< str<< " and "<<receiver<<endl;
		cout<<"Newly created buffer for communication : "<<freebit<<endl;
		cout <<"Message has been sent to the process: " << receiver <<" from the process: " << str <<" using the newly created buffer." <<endl;
		cout<<"Sent Message : "<<message<<endl;
		kernel->machine->WriteRegister(2,freebit) ;
            }
            else{
            	cout<<"No free buffer space found. send message failed!"<<endl;
            	kernel->machine->WriteRegister(2,-1) ;
            }
   		 }	
		}
		else{	
			// bool isPresent=FALSE;
    	ListIterator<Buffer*> *it = new ListIterator<Buffer*>(kernel->processMap[receiver]->bufferList);
   		 for (; !it->IsDone(); it->Next()) {
   		 	if(it->Item()->getBufferId()==bufferId)
   		 	{
   		 		cout<<"Buffer already present for communication between "<<it->Item()->getSender()<<" and "<<it->Item()->getReceiver()<<". Added the new message to the existing buffer "<<it->Item()->getBufferId()<<endl;
   		 		cout<<"Sent Message : "<<message<<endl;
   		 		// isPresent=TRUE;
   		 		it->Item()->addMessages(message);
   		 	}
   		 }
  //  		 if(!isPresent){
		// Buffer *buffer=new Buffer();
		// buffer->setReceiver(receiver);
		// //string str(kernel->currentThread->getName());
		// buffer->setSender(str);
		// buffer->setBufferId(bufferId);
		// buffer->addMessages(message);
		// kernel->processMap[receiver]->bufferList->Append(buffer);
		// cout<<"Buffer not present. Created buffer for communication between "<< str<< " and "<<receiver<<endl;
		// cout<<"Created buffer id: "<< bufferId;
		// }
   		}
		
	}
	else{
		cout<<"Message Cannot be delivered as the receiver doesnot exists"<<endl;
		kernel->machine->WriteRegister(2,-1) ;
	}
	// cout<<"\n*****end of sendMessage Syscall Handler*****\n";
	(void) interrupt->SetLevel(oldLevel);
}
   



bool 
isMessageArrived(string senderName, string receiver){
if(!kernel->currentThread->bufferList->IsEmpty()){
		ListIterator<Buffer*> *it = new ListIterator<Buffer*>(kernel->currentThread->bufferList);
   		 	for (; !it->IsDone(); it->Next()) {
   		 		if(it->Item()->getSender()==senderName&&it->Item()->getReceiver()==receiver)
   		 		{
   		 		if(it->Item()->getMessages()!=""){	
   		 		cout<<"Messages received from "<<senderName<<" to "<<receiver<<" :"<<"through the Buffer id: "<<it->Item()->getBufferId()<<endl;
                printf("--------------------------------------------------------\n");
   		 		cout<<"RECEIVED MESSAGES: "<<it->Item()->getMessages()<<endl;
   		 		cout<<"---------------------------------------------------------"<<endl;
   		 		kernel->machine->WriteRegister(2,it->Item()->getBufferId()) ;
   		 		return TRUE;
   		 		}
   		 	}
   		 	return FALSE;
	}
	}
	else{
		return FALSE;	
		// cout<<"No existing buffer available from communication between "<< senderName<< " and "<<receiver<<endl;
		// int freebit=kernel->bufferBitmap->FindAndSet();
  //           if(freebit!=-1){
  //           	Buffer *buffer=new Buffer();
		// 		buffer->setReceiver(receiver);
		// 		buffer->setSender(senderName);
		// 		buffer->setBufferId(freebit);
		// 		kernel->processMap[receiver]->bufferList->Append(buffer);
		// 		cout<<"Created buffer with id "<<freebit<<" for communication between "<< senderName<< " and "<<receiver<<endl;
		// 		kernel->machine->WriteRegister(2,freebit) ;
		// 		return FALSE;
  //           }
  //           else{
  //           	cout<<"wait message system call failed as there is no space in buffer pool"<<endl;
  //           	kernel->machine->WriteRegister(2,-1);
  //           	return TRUE;
  //           }
	}
}

void 
MessageArrived(string senderName, string receiver){
	// printf("Inside messageArrived\n");
if(!kernel->currentThread->bufferList->IsEmpty()){
	// printf("bufferlist not empty\n");
	//printf("sender: %s\n", senderName.c_str());
	//printf("receiver: %s\n", receiver.c_str());
		ListIterator<Buffer*> *it = new ListIterator<Buffer*>(kernel->currentThread->bufferList);
   		 	for (; !it->IsDone(); it->Next()) {
   		 		if(it->Item()->getSender()==senderName&&it->Item()->getReceiver()==receiver)
   		 		{
   		 		//printf("hi0\n");

   		 		if(it->Item()->getMessages()!=""){	
   		 		//printf("hi1\n");

   		 		cout<<"Messages received from "<<senderName<<" to "<<receiver<<" :"<<" through the buffer "<<it->Item()->getBufferId()<<endl;
   		 		//printf("hi\n");
   		 		cout<<"---------------------------------------------------------------"<<endl;
   		 		cout<<"RECEIVED MESSAGES: "<<it->Item()->getMessages()<<endl;
   		 		cout<<"---------------------------------------------------------------"<<endl;
   		 		kernel->machine->WriteRegister(2,it->Item()->getBufferId()) ;
   		 		}
   		 		else{
   		 			cout<<"Message sent from kernel: No messages received from the expected sender "<<senderName<<endl;
   		 			kernel->machine->WriteRegister(2,-1) ;	
   		 		}
   		 		} 
   		 	}
	}
	else {
		// printf("inside messagearrived else\n");
		cout<<"No message received as there is no valid sender process"<<endl;
		kernel->machine->WriteRegister(2,-1) ;	
	}
}

void
WaitMessageHandler(string senderName, string msg, int bufferId){
	cout<<"\n*****WaitMessage Syscall Handler*****"<<endl;
	cout<<"------------------------------------------\n"<<endl;
		Interrupt *interrupt = kernel->interrupt;
		Scheduler *scheduler = kernel->scheduler;
		IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
		string receiver(kernel->currentThread->getName());

		if(!isMessageArrived(senderName,receiver)){
			kernel->waitMessageThreads->Append(kernel->currentThread);
			cout<<"No message received yet from the sender-"<<senderName<<". Receiver-"<<receiver<<" going to sleep till the message arrives"<<endl;
			kernel->currentThread->Sleep(FALSE);
			cout<<"\nReceiver thread of the process"<<receiver<<" woke up from sleep"<<endl;
			MessageArrived(senderName,receiver);
		}
    cout<<"\n*****End of WaitMessage Syscall Handler*****\n";    
        (void) interrupt->SetLevel(oldLevel);
        /* Modify return point */
        {
            /* set previous programm counter (debugging only)*/
            kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

            /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
            kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

            /* set next programm counter for brach execution */
            kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
        }
}

bool 
isAnswerArrived(int id){
if(!kernel->currentThread->bufferList->IsEmpty()){
		ListIterator<Buffer*> *it = new ListIterator<Buffer*>(kernel->currentThread->bufferList);
   		 	for (; !it->IsDone(); it->Next()) {
   		 		if(it->Item()->getBufferId()==id)
   		 		{
   		 		if(it->Item()->getAnswer()!=""){	
   		 		cout<<"Answer received from "<<it->Item()->getSender()<<" to "<<it->Item()->getReceiver()<<" :"<<endl;
   		 		printf("------------------------------------------------------\n");
   		 		cout<<"RECEIVED ANSWER: "<<it->Item()->getAnswer()<<endl;
   		 		printf("------------------------------------------------------\n");

   		 		//cout<<"---------------------------------------------------------------"<<endl;
   		 		//kernel->machine->WriteRegister(2,it->Item()->getBufferId()) ;
   		 		return TRUE;
   		 		}
   		 		}
   		 	}
   		 	return FALSE;
	}
}




void
WaitAnswerHandler(int bufferId){
	//cout<<"\n*****WaitAnswer Syscall Handler*****\n"<<endl;
		Interrupt *interrupt = kernel->interrupt;
		Scheduler *scheduler = kernel->scheduler;
		IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
		//string receiver(kernel->currentThread->getName());

		if(!isAnswerArrived(bufferId)){
			//kernel->waitMessageThreads->Append(kernel->currentThread);
			kernel->waitAnswerThreads->Append(kernel->currentThread);
		cout<<"No answer received yet from the sender. Receiver thread of the process "<<kernel->currentThread->getName()<<" going to sleep till the answer arrives"<<endl;			
			kernel->currentThread->Sleep(FALSE);
			cout<<"Receiver thread of the process "<<kernel->currentThread->getName()<<" woke up from sleep"<<endl;
			if(!kernel->currentThread->bufferList->IsEmpty()){
			ListIterator<Buffer*> *it = new ListIterator<Buffer*>(kernel->currentThread->bufferList);
   		 	for (; !it->IsDone(); it->Next()) {

   		 		if(it->Item()->getBufferId()==bufferId)
   		 		{
   		 		if(it->Item()->getAnswer()!=""){	
   		 		cout<<"Answer received from "<<it->Item()->getSender()<<" to "<<it->Item()->getReceiver()<<" :"<<endl;
   		 		printf("------------------------------------------------------\n");
   		 		cout<<"RECEIVED ANSWER: "<<it->Item()->getAnswer()<<endl;
   		 		printf("------------------------------------------------------\n");

   		 		}
   		 		else{
   		 		printf("------------------------------------------------------\n");
   		 		cout<<"Message received from kernel: No Answer received from "<<it->Item()->getSender()<<" to "<<it->Item()->getReceiver()<<" :"<<endl;
   		 		printf("------------------------------------------------------\n");
   		 		
   		 		//cout<<"Received answer: "<<it->Item()->getAnswer()<<endl;	
   		 		}
   		 		}
   		 	}
		}
		}

    //cout<<"\n*****End of WaitAnswer Syscall Handler*****\n";    
        (void) interrupt->SetLevel(oldLevel);
        /* Modify return point */
        {
            /* set previous programm counter (debugging only)*/
            kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

            /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
            kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

            /* set next programm counter for brach execution */
            kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
        }
}




void
SendAnswerHandler(int result,string ans,int bufferId){
	Interrupt *interrupt = kernel->interrupt;
		Scheduler *scheduler = kernel->scheduler;
		IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);

if(!kernel->currentThread->bufferList->IsEmpty()){
	bool isSendSuccess=FALSE;
	ListIterator<Buffer*> *it = new ListIterator<Buffer*>(kernel->currentThread->bufferList);
   		 for (; !it->IsDone(); it->Next()) {

   		 	//printf("bufferId: %d\n", bufferId);
   		 	// printf("it->Item()->getBufferId(): %d\n", it->Item()->getBufferId());
   		 if(it->Item()->getBufferId()==bufferId)
   		 	{
   		 		Buffer *buffer=new Buffer();
				buffer->setReceiver(it->Item()->getSender());
				buffer->setSender(it->Item()->getReceiver());
				buffer->setBufferId(bufferId);
				buffer->setAnswer(ans);
				//kernel->processMap[receiver]->bufferList->Append(buffer);
   		 	
   		 	isSendSuccess=TRUE;
   		 	kernel->processMap[it->Item()->getSender()]->bufferList->Append(buffer);

   		 	cout<<"Answer sent to "<<it->Item()->getSender()<<" from "<<it->Item()->getReceiver()<<" through the buffer "<<bufferId<<"."<<endl;
   		 	cout<<"Answer sent: "<<ans<<endl;	
   		 	kernel->currentThread->bufferList->Remove(it->Item());
   		 	}
		
   		 }

   		 if(!isSendSuccess){
   		 	cout<<"Permission denied.! Sending answer failed. The buffer id is invalid."<<endl;
   		 } 
   	}

  (void) interrupt->SetLevel(oldLevel);

}





void
ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which) {
	case PageFaultException:
	{
		int badvadd = kernel->machine->ReadRegister(39);
		PageFaultHandler(badvadd);
	}
	return;
	break;
	case SyscallException:
	  switch(type) {
	  case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;

	  case SC_Add:
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();

	break;
		case FORK_POS:
		{
		cout<<"\n*****FORK_POS Syscall Handler*****\n";
		cout<<"----------------------------------\n"<<endl;	

	int val= (int)kernel->machine->ReadRegister(4);
	Thread *childThread = new Thread("Forked Thread");
	int ID=childThread->getID();
	if(val==1){
	childThread->Fork((VoidFunctionPtr) ForkTest1,(void *) ID);
	}
	if(val==2){
	childThread->Fork((VoidFunctionPtr) ForkTest2,(void *) ID);
	}
	if(val==3){
	childThread->Fork((VoidFunctionPtr) ForkTest3,(void *) ID);
	}
	cout<<"Forked thread "<<ID<<endl;
	kernel->machine->WriteRegister(2, (int)ID);
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	cout<<"\nIncreased PC to execute next instruction"<<endl;
	}
	return;
	
	ASSERTNOTREACHED();
	break;

		case WAIT_POS:{
			cout<<"\n*****WAIT_POS Syscall Handler*****"<<endl;
			cout<<"----------------------------------\n"<<endl;
	int childID=(int)kernel->machine->ReadRegister(4);
	 Interrupt *interrupt = kernel->interrupt;
	Scheduler *scheduler = kernel->scheduler;
	IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	threadMap[childID]=kernel->currentThread;
	cout<<"Making current thread to sleep"<<endl;
	kernel->currentThread->Sleep(false);
	(void) interrupt->SetLevel(oldLevel);
	//kernel->machine->WriteRegister(2, (int)childID);
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  // set programm counter to next instruction (all Instructions are 4 byte wide)
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}
	}
	return;
	
	ASSERTNOTREACHED();
	break;
		case SC_Write:
		{
			cout<<"\n*****Write Syscall Handler*****"<<endl;
			cout<<"----------------------------------\n"<<endl;
			Interrupt *interrupt = kernel->interrupt;
			Scheduler *scheduler = kernel->scheduler;
			IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
			cout<<"Getting values from read registers\n"<<endl;
			int startLoc=(int)kernel->machine->ReadRegister(4);
			int numOfCharToRead=(int)kernel->machine->ReadRegister(5);
			char output;
			int size=startLoc+numOfCharToRead;
			while(startLoc<size){
				bool isMem = kernel->machine->ReadMem(startLoc, 1, (int *)&output);
				if(isMem){
					cout<<output;
					startLoc++;
				}
			}
			cout<<endl;
			cout<<"\nIncreasing PC to execute next instruction"<<endl;
			(void) interrupt->SetLevel(oldLevel);
			{
			  /* set previous programm counter (debugging only)*/
			  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			  // set programm counter to next instruction (all Instructions are 4 byte wide)
			  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);	  
			  /* set next programm counter for brach execution */
			  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			}
		}
		return;
		break;

		case SC_Exit:{

			cout<<"\n*****SC_Exit Syscall *****"<<endl;
			cout<<"-----------------------------------"<<endl;
			cout<<"The thread of "<<kernel->currentThread->getName()<<" finished its execution."<<endl;
			for(map <string, Thread*>:: iterator iter= kernel->processMap.begin(); iter !=kernel->processMap.end(); iter++) {
            if(iter->second!=NULL){
            	Thread* thread=iter->second;
                if(thread!=NULL){
                	if(thread==kernel->currentThread){
                		ListIterator<Buffer*> *itb = new ListIterator<Buffer*>(thread->bufferList);
   					 	bool check=FALSE;
   		 				for (; !itb->IsDone(); itb->Next()) {
   		 					if(!(kernel->waitMessageThreads->IsInList(kernel->processMap[itb->Item()->getSender()])||kernel->waitAnswerThreads->IsInList(kernel->processMap[itb->Item()->getSender()])||kernel->scheduler->IsInReadyList(kernel->processMap[itb->Item()->getSender()]))){
   		 					if(itb->Item()->getBufferId()!=-1){
                		cout<<"Deallocated Buffer: "<<itb->Item()->getBufferId()<<" of the communication channel between the processes - "<<itb->Item()->getSender()<<" and "<<itb->Item()->getReceiver()<<", As both the assosiated processses finished its execution."<<endl;
                		kernel->bufferBitmap->Clear(itb->Item()->getBufferId());
                		}
                		 kernel->processMap.erase(iter);
                		}
                		}
                	}
                }
            }
        	}
        	cout<<"\n*****End of SC_Exit Syscall *****\n"<<endl;
        }
			kernel->currentThread->Finish();
		return;
		break;
		case SEND_MESSAGE:
		{
				cout<<"\n*****SendMessage Syscall Handler*****"<<endl;
			cout<<"------------------------------------------\n"<<endl;

			int receiver = (int) kernel->machine->ReadRegister(4);
            int message = (int) kernel->machine->ReadRegister(5);
            int bufferId = (int) kernel->machine->ReadRegister(6);
            // if(bufferId!=-1){
            	SendMessageHandler(receiver,message,bufferId);
        	// }
            // else{
            // 	string sender(kernel->currentThread->getName());
            // 	int bfr=checkForBufferId(sender,receiver);
            // 	if(bfr!=-1){
            // 		SendMessageHandler(receiver,message,bfr);
            // 	}
            // 	else{
            // 	int freebit=kernel->bufferBitmap->FindAndSet();
            // 	if(freebit!=-1){
            // 	SendMessageHandler(receiver,message,freebit);
            // 	}
            // 	else{
            // 	kernel->machine->WriteRegister(2, freebit);
            // 	cout<<"Cannot request send message system call as there is no space in buffer pool";
            // 	}
            // 	}
            // }
            	cout<<"\n*****End of SendMessage Syscall Handler*****\n"<<endl;
            {
        /* set previous programm counter (debugging only)*/
        kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
        /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
        kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
        /* set next programm counter for brach execution */
        kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
    		}
		}
		return;
		break;
		case WAIT_MESSAGE:
		{
			// cout<<"\n*****WaitMessage Syscall Handler*****\n"<<endl;
			int sender = (int) kernel->machine->ReadRegister(4);
            int message = (int) kernel->machine->ReadRegister(5);
            int bufferId = (int) kernel->machine->ReadRegister(6);
            string senderName=convertIntToString(sender);
            string msg=convertIntToString(message);
            WaitMessageHandler(senderName,msg,bufferId);
                        	// cout<<"\n*****End of SendMessage Syscall Handler*****\n"<<endl;

         //    if(bufferId!=-1){
         //    	WaitMessageHandler(senderName,msg,bufferId);
        	// }
         //    else{
         //    	int foundBuffer=findBufferId(senderName);
         //    if(foundBuffer!=-2){
         //    	cout<<"Buffer found for communication between "<< senderName<<" and "<< str<<endl;
         //    	WaitMessageHandler(senderName,msg,foundBuffer);
         //    }
         //    else{
         //    int freebit=kernel->bufferBitmap->FindAndSet();
         //    if(freebit!=-1){
         //    	string str(kernel->currentThread->getName());
         //    	cout<<"New buffer is created in buffer pool for communication between "<< senderName<<" and "<< str<<endl;
         //    	WaitMessageHandler(senderName,msg,freebit);
         //    }
         //    else{
         //    	kernel->machine->WriteRegister(2, freebit);
         //    	cout<<"Cannot request wait message system call as there is no space in buffer pool";
         //    }
         //    }
         //    }
      //       {
      //   /* set previous programm counter (debugging only)*/
      //   kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
      //   /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
      //   kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
      //   /* set next programm counter for brach execution */
      //   kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
    		// }

		// int port = (int) kernel->machine->ReadRegister(4);
		// 	WaitMessageHandler(port);
		}
		return;
		break;
		case SEND_ANSWER:
		{
				cout<<"\n*****SendAnswer Syscall Handler*****"<<endl;
				cout<<"-------------------------------------\n"<<endl;
			int result = (int) kernel->machine->ReadRegister(4);
            int answer = (int) kernel->machine->ReadRegister(5);
            int bufferId = (int) kernel->machine->ReadRegister(6);
            string ans=convertIntToString(answer);
            if(bufferId!=-1){
            	SendAnswerHandler(result,ans,bufferId);
            }
            else{
            	cout<<"Invalid buffer id. Cannot send answer using this buffer id"<<endl;
            }
            	cout<<"\n*****End of SendAnswer Syscall Handler*****\n"<<endl;

            {
        /* set previous programm counter (debugging only)*/
        kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
        /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
        kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
        /* set next programm counter for brach execution */
        kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
    		}
		}
		return;
		break;
		case WAIT_ANSWER:{
            cout<<"\n*****WaitAnswer Syscall Handler*****"<<endl;
				cout<<"-------------------------------------\n"<<endl;
			int result = (int) kernel->machine->ReadRegister(4);
            int answer = (int) kernel->machine->ReadRegister(5);
            int bufferId = (int) kernel->machine->ReadRegister(6);
            printf("buffer id from input: %d\n", bufferId);
            WaitAnswerHandler(bufferId);
		}
		return;
		break;

	  default:
	cerr << "Unexpected system call " << type << "\n";
	break;
	  }
	  break;
	default:
	  cerr << "Unexpected user mode exception" << (int)which << "\n";
	  break;
	}
	ASSERTNOTREACHED();
}


