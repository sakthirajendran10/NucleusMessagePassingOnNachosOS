#include "syscall.h"

int
main()
{
    
	char* message1 = "Hi from cprogram1. This is my first message.";
	char* message2 = "Hey, This is my second message.";
	char* message3 = "Finally, This is my last message.";

	char* receiver ="../test/cprogram2";
	int result1 = 0;
	int answer1 = "";
	int bufferId1=-1;
	
	bufferId1=SendMessage(receiver,message1,-1);
	SendMessage(receiver,message2,-1);
	if(bufferId1!=-1)
	{
		SendMessage(receiver,message3,bufferId1);
		WaitAnswer(result1,answer1,bufferId1);
	}   

}