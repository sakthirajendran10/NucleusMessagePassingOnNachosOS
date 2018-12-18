#include "syscall.h"

int
main()
{
    
	char* message = "Hi from program 3";
	char* answer1="hello program1";
	char* answer2="hello program2";

	char* sender1 ="../test/program1";
	char* sender2 ="../test/program2";
	int bufferId1 = -1;
	int bufferId2 = -1;

	int result=0;

	bufferId1=WaitMessage(sender1,message,-1);
	if(bufferId1!=-1)
	{
		SendAnswer(result,answer1,bufferId1);
	} 

	bufferId2=WaitMessage(sender2,message,-1);
	if(bufferId2!=-1)
	{
		SendAnswer(result,answer2,bufferId2);
	} 
	
	Exit(0);
}