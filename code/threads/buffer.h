#include "list.h"
#include <string>

class Buffer{
    private:
       //  string messagesReceived;
       // List<Thread*> *senderThreads;
       //  Thread* receiverThread;
        string receiver;
        string sender;
        int bufferId;
        string messages;
        string answer;
    public:

        // List<string> *messages;
        // List<string> *answers;
        Buffer(){
            //senderThreads=new List<Thread*>();
            // messages=new List<string>();   
            // answers=new List<string>();
        };
        // void addMessagesReceived(string message);
        // void clearMessagesReceived();
        // string getMessagesReceived();
        // List<Thread*>* getSenderThreads();
        // void addSenderThread(Thread* thread);
        // Thread* getReceiverThread();
        // void setReceiverThread(Thread* thread);
        //void clearMessagesReceived();
        //List<Thread*>* getSenderThreads();

        void addMessages(string message);
        void clearMessages();
        string getMessages();
        void setAnswer(string ans);
        void clearAnswer();
        string getAnswer();
        void setBufferId(int id);
        int getBufferId();
        
        void setReceiver(string rec);
        string getReceiver();
        void setSender(string sen);
        string getSender();
        
        // void setReceiverThread(Thread* thread);
    };