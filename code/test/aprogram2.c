#include "syscall.h"

int
main()
{
    
	char* message = "Hi from aprogram2";
	char* message1 = "";

	char* answer1="Hi aprogram1, from aprogram2";
	int result = 0;
	int bufferId1=-1;

	char* sender1 = "../test/aprogram1";

	bufferId1=WaitMessage(sender1,message1,-1);
	if(bufferId1!=-1)
	{
		SendAnswer(result,answer1,bufferId1);
	} 
	
}