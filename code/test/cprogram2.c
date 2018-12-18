#include "syscall.h"

int
main()
{
    
	char* message = "";
	int result = 0;
	int bufferId=-1;
	char* answer="Hello cprogram1, All of your messages have been received";
	char* sender = "../test/cprogram1";

	bufferId=WaitMessage(sender,message,-1);
	if(bufferId!=-1)
	{
		SendAnswer(result,answer,bufferId);
	} 
	
}