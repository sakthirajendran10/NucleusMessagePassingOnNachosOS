#include "syscall.h"

int
main()
{
    
	char* message = "Hi from aprogram1";
	char* receiver1 ="../test/aprogram2";
	char* receiver2 ="../test/aprogram3";
	
	int result1=0;
	char* answer0="Hi aprogram4, from aprogram1";
	char* answer1="";
	char* answer2= "";
	int result=0;
	int result2=0;
	//char* answer2=0;
	int bufferId0=-1;
	int bufferId1=-1;
	int bufferId2=-1;

	char* sender1 = "../test/aprogram4";

	bufferId0=WaitMessage(sender1,message,-1);
	if(bufferId0!=-1)
	{
		SendAnswer(result,answer0,bufferId0);
	} 

	bufferId1=SendMessage(receiver1,message,-1);
	if(bufferId1!=-1)
	{
		WaitAnswer(result1,answer1,bufferId1);
	} 

	bufferId2=SendMessage(receiver2,message,-1);
	if(bufferId2!=-1)
	{
		WaitAnswer(result2,answer2,bufferId2);
	}   


	Exit(0);
}