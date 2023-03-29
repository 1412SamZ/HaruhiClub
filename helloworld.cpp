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


    CPU_OCP cpu1;
    CPU_OCP cpu2;
    while(1)
    {    getCpuOccupy(&cpu1);
        sleep(1);
        getCpuOccupy(&cpu2);
        double cpuUsed = calCpuOccupy(&cpu1, &cpu2);

        printf("testing...\n");

        cout << cpuUsed << endl;}
    
    // serverTCP stcp;
    // stcp.stcpListen(dport);
    // struct sockaddr_in caddr;

    // TcpSocket* ts (stcp.stcpAccept(&caddr));
    
    // ts->sendMsg("Hi Server There!");


    // std::string rstr = ts->recvMsg();
    // std::cout << rstr << std::endl;
    // return 0;


}