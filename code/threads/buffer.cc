#include "buffer.h"

void Buffer::addMessages(string message){
    messages=messages+" "+message;
}
string Buffer::getMessages(){
    return messages;
}
string Buffer::getAnswer(){
    return answer;
}
void Buffer::setAnswer(string ans){
    answer =ans;
}
void Buffer::clearMessages(){
	messages="";	
}
void Buffer::clearAnswer(){
	answer="";	
}
void Buffer::setBufferId(int id){
    bufferId=id;
}
int Buffer::getBufferId(){
    return bufferId;
}
void Buffer::setReceiver(string rec){
    receiver = rec;
}
string Buffer::getReceiver(){
	return receiver;
}
void Buffer::setSender(string sen){
	sender=sen;
}
string Buffer::getSender(){
	return sender;
}
        