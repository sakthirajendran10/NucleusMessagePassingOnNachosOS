#include "syscall.h"

int
main()
{
    
	//char* message="Hi from program 2";
	char* message = "Hi from program 2";
	char* answer="hello program1";
	char* sender1 ="../test/program1";
	char* receiver1="../test/program3";
	int result=0;


	// int bufferId=WaitMessage(sender1,message,-1);
	// if(bufferId!=-1)
	// {
	// 	SendAnswer(result,answer,bufferId);
	// } 

	int bufferId1=SendMessage(receiver1,message,-1);
	if(bufferId1!=-1)
	{
		WaitAnswer(result,answer,bufferId1);
	}   
	Exit(0);
}