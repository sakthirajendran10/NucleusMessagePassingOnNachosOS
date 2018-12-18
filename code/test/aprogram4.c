#include "syscall.h"

int
main()
{
    
	char* message = "Hi from aprogram 4";
	char* receiver1 ="../test/aprogram1";


	int result=0;
	char* answer="";
	
	int bufferId=SendMessage(receiver1,message,-1);
	if(bufferId!=-1)
	{
		WaitAnswer(result,answer,bufferId);
	}     
	Exit(0);
}