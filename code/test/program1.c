#include "syscall.h"

int
main()
{
    
	char* message = "Hi from program 1";
	//int result=PortAssign(1);
	char* receiver1 ="../test/program3";
	int result=0;
	char* answer="";
	int bufferId=SendMessage(receiver1,message,-1);
	if(bufferId!=-1)
	{
		WaitAnswer(result,answer,bufferId);
	}     
	Exit(0);
}