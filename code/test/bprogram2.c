#include "syscall.h"

int
main()
{
    
	char* message = "";
	int result = 0;
	int bufferId=-1;
	char* answer="Hi bprogram1, from bprogram2";
	char* sender = "../test/bprogram1";

	bufferId=WaitMessage(sender,message,-1);
	if(bufferId!=-1)
	{
		SendAnswer(result,answer,bufferId);
	} 
	
}