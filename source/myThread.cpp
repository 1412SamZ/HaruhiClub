#include "myThread.h"



void pidThread(TcpSocket* stcp, int curfd)
{
    while(!pidThreadStop)
    {
        sleep(1);
        string retword = listDir();
        v2msg msg("server", retword);
        stcp->SendMsgV2(msg, curfd);

    }
    cout << "pidThread finished" << endl;
}

void fileThread(TcpSocket* stcp, int curfd)
{
    


}