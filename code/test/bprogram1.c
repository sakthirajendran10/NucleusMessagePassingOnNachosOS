#include "syscall.h"

int
main()
{
    
	char* message = "Hi from bprogram1";
	char* receiver ="../test/bprogram2";
	int result1 = 0;
	int answer1 = "";
	int bufferId1=-1;
	
	bufferId1=SendMessage(receiver,message,-1);
	if(bufferId1!=-1)
	{
		WaitAnswer(result1,answer1,bufferId1);
	}   

}