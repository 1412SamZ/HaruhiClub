#include"include/mySocket.h"
#include"include/myWheels.h"
#include"include/mySys.h"
#include <string>
#include<iostream>
#include"include/sqlpp.h"
#include<sys/epoll.h>
#include<dirent.h>
#include<cstring>

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include<thread>



#define dport 9999
#define EPOLL_SIZE 1024

using namespace std;
void sampleThread()
{
    Mapi mc;
    bool connected = mc.connect("root", "Zhanggy123", "", "localhost", 0);
    cout << "SQL SERVER CONNECTION STATUS: " << connected << endl;
    string ccc;
    // if(
    //     mc.query(
    //         "use database BGManagament"
    //     )
    // )cout << "use database failed" << endl;
    // // if(
    // //     mc.query(
    // //         "use table login"
    // //     )
    // // )cout << "use table failed" << endl;

    // string cmd = "select * from login";

    while(1){
        if(mc.next()){
            ccc = mc.value();
            cout << ccc << endl;
        }
        else break;
    }

    return;

}







bool isPid(const char* pidStr)
{
    for(int i = 0; i < strlen(pidStr); i++)
    {
        if(!isdigit(pidStr[i]))return false;
    }
    return true;
}




void printNetIo()
{
    char path[] = "/proc/net/dev";
    FILE* netDevFile = fopen(path, "r");
    char buff[1024];

    while(fgets(buff, sizeof(buff), netDevFile))cout << buff << endl;
    
    // scanf(buff, "%s", buff);
    return;

}



#include <map>
#include <vector>

std::string threadVm(string pid)
{
    string procPath = "/proc/" + pid + "/status";
    FILE* fp1 = fopen(procPath.c_str(), "r");
    if(fp1 == NULL)return "";
    char str[300];
    fgets(str, 300, fp1);
    
    int k = 21;
    while(k--){fgets(str, 300, fp1);}
    //cout << str << endl;
    int ret = 0;
    sscanf(str, "VmRSS: %d kB", &ret);
    //cout << ret << endl;
    fclose(fp1);
    return std::to_string(ret);
}



std::map<string, std::vector<unsigned long>> mp;
std::string threadCpu(string pid) {
    string procStatPath = "/proc/" + pid + "/stat";
    ifstream procStatFile(procStatPath.c_str());

    // if (!procStatFile.is_open()) {
    //     //cout << "Failed to open file " << procStatPath << endl;
        
    // }

    string line;
    getline(procStatFile, line);

    stringstream lineStream(line);
    string token;

    // 取出文件中第14个字段（utime）和第15个字段（stime），它们是该进程在用户态和kernel态下运行的时间
    for (int i = 1; i <= 13; i++) {
        lineStream >> token;
    }
    unsigned long utime, stime;
    lineStream >> utime >> stime;
    procStatFile.close();

    if(mp.find(pid) == mp.end())mp[pid] = {utime,stime,utime,stime};


    mp[pid][2] = mp[pid][0];
    mp[pid][3] = mp[pid][1];
    mp[pid][0] = utime;
    mp[pid][1] = stime;



    unsigned long total_time = mp[pid][0] + mp[pid][1] - mp[pid][2] - mp[pid][3];
    float cpu_usage = ((float)total_time / sysconf(_SC_CLK_TCK)) * 100;

    //cout << pid << " " << cpu_usage << " ";
    return  pid + " " + std::to_string(cpu_usage) + " ";   

    
}



std::string listDir(const string& path = "/proc")
{
    struct dirent* dirProc;
    DIR* dp;
    DIR* pdp;
    dp = opendir(path.c_str());
    if(dp == NULL)perror("Error: proc reading failed!");



    std::string ret;
    int count = 0;
    while((dirProc = readdir(dp)))
    {
        if(!isPid(dirProc -> d_name))continue;

        // cout << dirProc->d_name << endl;
        ret += threadCpu(dirProc->d_name);
        //ret += threadVm(dirProc -> d_name);
        
        ret += "\n";
        count ++;
        // break;
    }
    // cout << count << endl;
    return ret;


}

bool pidThreadStop = false;

void pidThread()
{
    int i = 30;
    
    std::string str;
    while(!pidThreadStop)
    {
        sleep(1);
        if(pidThreadStop)break;
        str = listDir();
        cout << str << endl;
    }
    cout << "pidThread finished" << endl;
}


void loginCheck(std::string username, std::string password)
{
    Mapi mc;
    bool connected = mc.connect("root", "Zhanggy123", "", "localhost", 0);
    cout << "SQL SERVER CONNECTION STATUS: " << connected << endl;
    string ccc;
    
        mc.update(
            "use BGManagement"
        );


    //cmd = "show databases";
    string cmd = "select password from login where username = 'moon'";
    mc.query(cmd);    
    while(1){
        if(mc.next()){
            ccc = mc.value();
            cout << ccc << endl;
        }
        else break;
    }
    cout << "thread end" << endl;
    return;

}





using namespace std;
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
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 本地多有的ＩＰ
    
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


    bool pidThreadRunning = false;



    while(1)
    {
        // 调用一次, 检测一次
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
                    exit(0);
                }
                cout << "client connected" << endl;
            }
            else
            {
                // 处理通信的文件描述符
                // 接收数据
                //
                TcpSocket* t = new TcpSocket(59);

                
                string username = "test user";              
                string msgStructStr = "This is a message asdasdasdasd";
                v2msg msg(username, msgStructStr);
                msg.msgType = MSGTYPE_PRIVATE;
                msg.requestType = REQUEST_LOGIN;
                
                // msg.msgLength = msgStructStr.size();
                // // msg.msg = msgStructStr.c_str();

                // // header
                //     msg.header = sizeof(msg.requestType)
                //     + sizeof(msg.username)
                //     + sizeof(msg.msgType)
                //     + sizeof(msg.msgLength)
                //     + msg.msgLength;
                t->SendMsgV2(msg, curfd);
                





                char buf[1024];
                memset(buf, 0, sizeof(buf));
                int len = recv(curfd, buf, sizeof(buf), 0);
                std::string str = buf;
                std::cout << str << endl;
                if(len == 0)
                {
                    printf("客户端已经断开了连接\n");
                    // 将这个文件描述符从epoll模型中删除
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                }
                else if(len > 0)
                {

                    printf("客户端 %d say: %s\n", curfd,buf);
                    string cmdStr(buf);
                    if(cmdStr == "CLOSE!")return 0;
                    if(cmdStr == "login")
                    {
                        std::thread loginThread(loginCheck, "moon", "moon");
                        loginThread.join();
                        std::cout << " thread joined!" << std::endl;
                        continue;
                    }
                    if(cmdStr == "CPU" && !pidThreadRunning)
                    {
                        pidThreadStop = false;
                        // thread pidt(pidThread);
                        // pidt.detach();
                        // pidThreadRunning = true;
                        int i = 10;
                        while(i--)
                        {
                            sleep(1);
                            string retword = listDir();
                            cout << "retlen: " << retword.size() << endl;
                            int sendLen = send(curfd, retword.c_str(), retword.size(), 0);
                            cout << sendLen << endl;
                        }
                        // string retword = listDir();
                        // // cout << retword << endl;
                        // send(curfd, retword.c_str(), retword.size(), 0);
                        cout << "cmd return sent! " << endl;
                    }
                    else if(cmdStr == "CPUCLOSE")
                    {
                        pidThreadRunning = false;
                    }
                    //send(curfd, buf, len, 0);
                    // 开一个线程 调用函数，返回结果
                }
                else
                {
                    perror("recv");
                    // exit(0);
                } 
            }
        }
    }

    return 0;


    // sampleThread();
    // string cmd = "cat /proc/stat";
    
    // string result;

    // result = ExecuteCMD(cmd.c_str());


    // CPU_OCP cpu1;
    // CPU_OCP cpu2;
    // while(1)
    // {   getCpuOccupy(&cpu1);
    //     sleep(1);
    //     getCpuOccupy(&cpu2);
    //     double cpuUsed = calCpuOccupy(&cpu1, &cpu2);

    //     printf("testing...\n");

    //     cout << cpuUsed << endl;
    // }
    


    // struct sockaddr_in caddr;

    // TcpSocket* ts (stcp.stcpAccept(&caddr));
    
    // ts->sendMsg("Hi Server There!");

    // std::string rstr;
    // while(1){
    //     rstr = ts->recvMsg();
    //     std::cout << rstr << std::endl;
    // }






    return 0;


}