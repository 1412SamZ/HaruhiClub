#include"include/mySocket.h"
#include"include/myWheels.h"
#include"include/mySys.h"
#include <string>
#include<iostream>
#define dport 9999


using namespace std;
int main(){

    
    // string cmd = "cat /proc/stat";
    
    // string result;

    // result = ExecuteCMD(cmd.c_str());


    CPU_OCCUPY cpu;
    pcpu(&cpu);

    printf("testing...\n");
    cout << endl;

    
    // serverTCP stcp;
    // stcp.stcpListen(dport);
    // struct sockaddr_in caddr;

    // TcpSocket* ts (stcp.stcpAccept(&caddr));
    
    // ts->sendMsg("Hi Server There!");


    // std::string rstr = ts->recvMsg();
    // std::cout << rstr << std::endl;
    // return 0;


}