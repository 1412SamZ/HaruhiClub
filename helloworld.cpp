#include"mySocket.h"
#include"include/myWheels.h"
#include"include/sqlpp.h" 
#include "myThread.h"
#include"myUtils.h"
#include <string>
#include<iostream>
#include<sys/epoll.h>
#include<dirent.h>
#include<cstring>

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include<thread>
#include<regex>
#include <unordered_map>

#define dport 9999
#define EPOLL_SIZE 1024



using namespace std;








bool pidThreadStop = false;
bool pidThreadRunning = false;
TcpSocket* stcp = nullptr;



std::unordered_map<std::string, int> cmdMp{
    {"login",1},
    {"cpu", 2},
    {"ls", 3},
    {"top", 4}
};

std::unordered_map<int, std::string> loginUsers;

int main(int argc, const char* argv[])
{

    // 创建监听的套接字
    
    pid_t curPid = getpid();
    cout << "Pid: " << curPid <<endl <<"Server Starting..."<< endl;
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
    {
        perror("socket error");
        exit(1);
    }

    // 绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  
    
    // 设置端口复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定端口
    int ret = bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(ret == -1)
    {
        perror("bind error");
        exit(1);
    }

    // 监听
    ret = listen(lfd, 64);
    if(ret == -1)
    {
        perror("listen error");
        exit(1);
    }

    // 现在只有监听的文件描述符
    // 所有的文件描述符对应读写缓冲区状态都是委托内核进行检测的epoll
    // 创建一个epoll模型
    int epfd = epoll_create(100);
    if(epfd == -1)
    {
        perror("epoll_create");
        exit(0);
    }

    // 往epoll实例中添加需要检测的节点, 现在只有监听的文件描述符
    struct epoll_event ev;
    ev.events = EPOLLIN;    // 检测lfd读读缓冲区是否有数据
    ev.data.fd = lfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if(ret == -1)
    {
        perror("epoll_ctl");
        exit(0);
    }

    struct epoll_event evs[1024];
    int size = sizeof(evs) / sizeof(struct epoll_event);
    // 持续检测




    stcp = new TcpSocket(lfd);

    while(1)
    {
        // 调用一次, 检测一次
        std::cout << "epoll waiting" << std::endl;
        int num = epoll_wait(epfd, evs, size, -1);
        for(int i=0; i<num; ++i)
        {
            // 取出当前的文件描述符
            int curfd = evs[i].data.fd;
            // 判断这个文件描述符是不是用于监听的
            if(curfd == lfd)
            {
                // 建立新的连接
                int cfd = accept(curfd, NULL, NULL);
                // 新得到的文件描述符添加到epoll模型中, 下一轮循环的时候就可以被检测了
                ev.events = EPOLLIN;    
                ev.data.fd = cfd;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                if(ret == -1)
                {
                    perror("epoll_ctl-accept");
                    //exit(0);
                }
                std::cout << "client connected" << endl;
            }
            else
            {
                // 处理通信的文件描述符

                // code表示执行结果
                int code;
                v2msg cms = stcp -> recvMsgStructV2(curfd, &code);
                std::string clientMsg = cms.msg;
                if(code == 0)
                {               
                    printf("客户端已经断开了连接\n");
                    // 将这个文件描述符从epoll模型中删除

                    pidThreadStop = true;
                    pidThreadRunning = false;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                    continue;
                }


                if(cms.requestType == REQUEST_FILE)
                {
                    if(cms.msgType == MSGTYPE_START_REQUEST)
                    {
                        v2msg msg("server", "");
                        std::string ans;
                        std::string lsCmd;
                        
                        lsCmd = "pwd";
                        ans = ExecuteCMD(lsCmd.c_str());
                        msg.setMessage(ans + "/");
                        stcp->SendMsgV2(msg, curfd);

                        lsCmd = "ls -l";
                        ans = ExecuteCMD(lsCmd.c_str());
                        msg.setMessage(ans);
                        stcp->SendMsgV2(msg, curfd);
                    }
                    else if(cms.msgType == MSGTYPE_UPDATE_REQUEST)
                    {
                        v2msg msg("server", "");
                        std::string ans;
                        std::string lsCmd;
                        
                        lsCmd = "pwd";
                        ans = ExecuteCMD(lsCmd.c_str());
                        msg.setMessage(clientMsg + "\n");
                        stcp->SendMsgV2(msg, curfd);

                        lsCmd = "ls -l ";
                        lsCmd += clientMsg;
                        ans = ExecuteCMD(lsCmd.c_str());
                        msg.setMessage(ans);
                        stcp->SendMsgV2(msg, curfd);
                    }
                    
                }


                continue;
                if(clientMsg == "pidThread")
                {
                    std::cout << "Starting threads"<<clientMsg << endl;
                
                    // create pidThread
                    thread pidt(pidThread, stcp, curfd);

                    // identify that this thread is running
                    pidThreadRunning = true;
                    pidt.detach();

                    std::cout << "pid thread started!" << std::endl;
                }




            }
        }
    }

    return 0;


}