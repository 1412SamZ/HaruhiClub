#include<iostream>
#include"include/mySys.h"
#include <unistd.h>
using namespace std;
#include <string>
#include "include/mySocket.h"
void getPidCpu()
{

}


int main()
{
    TcpSocket* t = new TcpSocket(5);
    v2msg msg;

    // username
    // string msgStructStr = "test user";
    // memset(msg.username, 0, sizeof msg.username);

    // for(int i = 0; i < msgStructStr.size(); i++)
    // {
    //     msg.username[i] = msgStructStr[i];
    // }

    // types
    msg.msgType = MSGTYPE_PRIVATE;
    msg.requestType = REQUEST_LOGIN;
    
    // msg and msg length
    std::string msgStructStr = "This is a message for v2msg testing";
    msg.msgLength = msgStructStr.size();
    msg.msg = msgStructStr.c_str();

    // header
    msg.header = sizeof(msg.requestType)
        + sizeof(msg.username)
        + sizeof(msg.msgType)
        + sizeof(msg.msgLength)
        + msg.msgLength;
    t->SendMsgV2(msg);

    return 0;
    
}